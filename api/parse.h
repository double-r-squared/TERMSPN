#pragma once
// api/parse.h — ESPN JSON parsers
// Add new parsers here when new endpoints are added.
// Depends on: types.h, http.h, api/urls.h, utils/json.hpp, utils/text.h
#include <string>
#include <vector>
#include <algorithm>
#include "../types.h"
#include "../http.h"
#include "urls.h"
#include "../utils/text.h"
#include "../utils/json.hpp"
using json = nlohmann::json;
using namespace std;

// ─── Parse helpers ────────────────────────────────────────────────────────────

static string findStat(const json& stats, const string& key) {
    for (auto& s : stats)
        if (s.value("type", "") == key)
            return s.value("summary", s.value("displayValue", "--"));
    return "--";
}

static string findStatValue(const json& stats, const string& name) {
    for (auto& s : stats)
        if (s.value("name", "") == name)
            return s.value("displayValue", "--");
    return "--";
}

static double findStatDouble(const json& stats, const string& name) {
    for (auto& s : stats)
        if (s.value("name", "") == name)
            return s.value("value", 0.0);
    return 0.0;
}

// ─── Parsers ──────────────────────────────────────────────────────────────────

vector<Team> parseStandings(const string& response, League league) {
    vector<Team> teams;
    if (response.empty()) return teams;
    json root = json::parse(response, nullptr, false);
    if (root.is_discarded()) return teams;

    for (auto& conference : root["children"]) {
        string confName = conference.value("name", "");
        for (auto& entry : conference["standings"]["entries"]) {
            Team team;
            auto& t = entry["team"];
            team.id           = t.value("id", "");
            team.displayName  = t.value("displayName", "");
            team.abbreviation = t.value("abbreviation", "");
            team.conference   = confName;

            auto& stats = entry["stats"];
            team.record    = findStat(stats, "total");
            team.home      = findStat(stats, "home");
            team.away      = findStat(stats, "road");
            team.divRecord = findStat(stats, "vsdiv");
            team.conf      = findStat(stats, "vsconf");
            team.lastTen   = findStat(stats, "lasttengames");
            team.pct       = findStatValue(stats, "winPercent");
            team.diff      = findStatValue(stats, "differential");
            team.streak    = findStatValue(stats, "streak");
            team.winPct    = findStatDouble(stats, "winPercent");

            switch (league) {
                case NBA:
                    team.ppg    = findStatValue(stats, "avgPointsFor");
                    team.oppPpg = findStatValue(stats, "avgPointsAgainst");
                    break;
                case NFL:
                    team.ppg    = findStatValue(stats, "pointsFor");
                    team.oppPpg = findStatValue(stats, "pointsAgainst");
                    break;
                default:
                    team.ppg    = findStatValue(stats, "avgPointsFor");
                    team.oppPpg = findStatValue(stats, "avgPointsAgainst");
                    break;
            }

            teams.push_back(team);
        }
    }

    sort(teams.begin(), teams.end(), [](const Team& a, const Team& b) {
        if (a.conference != b.conference) return a.conference < b.conference;
        return a.winPct > b.winPct;
    });

    return teams;
}

void enrichTeamDetail(Team& team, League league) {
    string response = fetchUrl(buildTeamUrl(league, team.id));
    if (response.empty()) return;
    json root = json::parse(response, nullptr, false);
    if (root.is_discarded()) return;
    team.standing = root["team"].value("standingSummary", "--");
}

// ─── News ─────────────────────────────────────────────────────────────────────

vector<NewsArticle> parseNewsList(const string& response) {
    vector<NewsArticle> articles;
    if (response.empty()) return articles;
    json root = json::parse(response, nullptr, false);
    if (root.is_discarded() || !root.contains("articles")) return articles;

    for (auto& a : root["articles"]) {
        // Skip video-only articles: they have images but none of type "header"
        // (only "Media" thumbnails). Real text articles always have a "header" image.
        const auto& imgs = a.value("images", json::array());
        if (!imgs.empty()) {
            bool hasHeader = false;
            for (auto& img : imgs)
                if (img.value("type", "") == "header") { hasHeader = true; break; }
            if (!hasHeader) continue;
        }

        NewsArticle article;
        article.id          = a.value("id", 0);
        article.headline    = a.value("headline", "");
        article.description = a.value("description", "");
        article.author      = a.value("byline", a.value("source", ""));
        article.published   = a.value("published", "");
        article.type        = a.value("type", "");
        article.premium     = a.value("premium", false);

        if (a.contains("links")) {
            if (a["links"].contains("web"))
                article.url = a["links"]["web"].value("href", "");
            if (a["links"].contains("api") && a["links"]["api"].contains("self"))
                article.contentApiUrl = a["links"]["api"]["self"].value("href", "");
        }

        for (auto& c : a.value("categories", json::array())) {
            string type = c.value("type", "");
            if      (type == "team")    article.teams.push_back(c.value("description", ""));
            else if (type == "athlete") article.athletes.push_back(c.value("description", ""));
        }

        articles.push_back(article);
    }
    return articles;
}

// Lazy-fetches the full story body from the content API.
// No-op if story is already populated or contentApiUrl is empty.
void enrichNewsStory(NewsArticle& article) {
    if (!article.story.empty() || article.contentApiUrl.empty()) return;
    string response = fetchUrl(article.contentApiUrl);
    if (response.empty()) return;
    json root = json::parse(response, nullptr, false);
    if (root.is_discarded()) return;

    // Content API returns the article object directly
    json& a = root.contains("headlines") ? root["headlines"][0] : root;

    if (article.author.empty())
        article.author = a.value("byline", a.value("source", ""));
    if (a.contains("story"))
        article.story = stripHtml(a.value("story", ""));
    else
        article.story = article.description;
}
