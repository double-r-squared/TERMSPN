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

// Returns "D. Booker (31)" for a given stat category name within a leaders array.
static string leaderStr(const json& leaders, const string& name) {
    for (const auto& cat : leaders) {
        if (cat.value("name", "") != name) continue;
        const auto& top = cat["leaders"];
        if (top.empty()) return "--";
        const auto& l = top[0];
        string n = l["athlete"].value("shortName", l["athlete"].value("displayName", "--"));
        return n + " (" + l.value("displayValue", "--") + ")";
    }
    return "--";
}

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

// ─── Schedule ─────────────────────────────────────────────────────────────────

vector<GameResult> parseSchedule(const string& response) {
    vector<GameResult> games;
    if (response.empty()) return games;
    json root = json::parse(response, nullptr, false);
    if (root.is_discarded() || !root.contains("events")) return games;

    string teamId = root["team"].value("id", "");

    for (const auto& event : root["events"]) {
        if (!event.contains("competitions") || event["competitions"].empty()) continue;
        const auto& comp = event["competitions"][0];

        GameResult g;
        g.date = event.value("date", "");

        if (comp.contains("status") && comp["status"].contains("type")) {
            const auto& st = comp["status"]["type"];
            g.state      = st.value("state", "pre");
            g.completed  = st.value("completed", false);
            g.statusText = st.value("shortDetail", "");
        }

        if (!comp.contains("competitors")) continue;

        // Separate our team from the opponent
        json ourComp, oppComp;
        bool foundOur = false, foundOpp = false;
        for (const auto& c : comp["competitors"]) {
            if (c.value("id", "") == teamId) { ourComp = c; foundOur = true; }
            else                              { oppComp = c; foundOpp = true; }
        }
        if (!foundOur || !foundOpp) continue;

        g.homeAway = ourComp.value("homeAway", "");
        g.opponent = oppComp["team"].value("abbreviation", "");
        g.oppName  = oppComp["team"].value("displayName", "");

        if (g.completed) {
            g.result = ourComp.value("winner", false) ? "W" : "L";
            if (ourComp.contains("score"))
                g.ourScore = ourComp["score"].value("displayValue", "");
            if (oppComp.contains("score"))
                g.oppScore = oppComp["score"].value("displayValue", "");

            for (const auto& r : ourComp.value("record", json::array()))
                if (r.value("type", "") == "total") { g.record = r.value("displayValue", "--"); break; }

            if (ourComp.contains("leaders")) {
                g.highPts = leaderStr(ourComp["leaders"], "points");
                g.highReb = leaderStr(ourComp["leaders"], "rebounds");
                g.highAst = leaderStr(ourComp["leaders"], "assists");
            }
        }

        games.push_back(g);
    }
    return games;
}
