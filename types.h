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
    double winPct = 0.0;
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
