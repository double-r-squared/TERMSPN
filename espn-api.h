#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <curl/curl.h>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;

enum League { NBA, NFL, LEAGUE_COUNT };
inline const string LEAGUE_NAMES[] = { "NBA", "NFL" };

struct Team {
    string id;
    string displayName;
    string abbreviation;
    string conference;
    string record;
    string pct;
    string ppg;
    string oppPpg;
    string diff;
    string streak;
    string home;
    string away;
    string divRecord;
    string conf;
    string lastTen;
    string standing;
    double winPct = 0.0;
};

// ─── HTTP ─────────────────────────────────────────────────────────────────────

static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    static_cast<string*>(userp)->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

string fetchUrl(const string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return "";
    return response;
}

// ─── URL builders ─────────────────────────────────────────────────────────────

const string SITE_API           = "https://site.api.espn.com/apis/site/v2/sports";
const string SITE_API_STANDINGS = "https://site.api.espn.com/apis/v2/sports";

string buildStandingsUrl(League league) {
    switch (league) {
        case NFL: return SITE_API_STANDINGS + "/football/nfl/standings";
        case NBA: return SITE_API_STANDINGS + "/basketball/nba/standings";
        default:  return "";
    }
}

string buildTeamUrl(League league, const string& teamId) {
    switch (league) {
        case NFL: return SITE_API + "/football/nfl/teams/" + teamId;
        case NBA: return SITE_API + "/basketball/nba/teams/" + teamId;
        default:  return "";
    }
}

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

// ─── Parsing ─────────────────────────────────────────────────────────────────

vector<Team> parseStandings(const string& response) {
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
            team.ppg       = findStatValue(stats, "avgPointsFor");
            team.oppPpg    = findStatValue(stats, "avgPointsAgainst");
            team.diff      = findStatValue(stats, "differential");
            team.streak    = findStatValue(stats, "streak");
            team.winPct    = findStatDouble(stats, "winPercent");

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