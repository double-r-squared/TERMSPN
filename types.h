#pragma once
// types.h — shared data structs and enums
// All other modules depend on this. No logic, no I/O.
#include <string>
#include <vector>
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
    string venue;
    double winPct = 0.0;
};

struct GameResult {
    string date;         // ISO 8601 UTC
    string opponent;     // abbreviation e.g. "SAC"
    string oppName;      // full name
    string homeAway;     // "home" or "away"
    string state;        // "pre" / "in" / "post"
    string statusText;   // "Final" / "7:00 PM ET" / "Q3 4:32"
    bool   completed  = false;
    string result;       // "W" or "L" — completed games only
    string ourScore;     // "120"
    string oppScore;     // "116"
    string record;       // team record after this game e.g. "1-0"
    string highPts;      // "D. Booker (31)"
    string highReb;      // "M. Williams (11)"
    string highAst;      // "G. Allen (7)"
};

struct NewsArticle {
    int    id            = 0;
    string headline;
    string description;
    string author;         // byline — may be empty on HeadlineNews
    string published;      // ISO 8601 UTC
    string type;           // "Story" or "HeadlineNews"
    string url;            // ESPN.com web link
    string contentApiUrl;  // content.core.api.espn.com — used to lazy-fetch story body
    string story;          // plain-text body, empty until enrichNewsStory() is called
    bool   premium = false;
    vector<string> teams;
    vector<string> athletes;
};
