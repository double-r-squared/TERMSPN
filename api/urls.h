#pragma once
// api/urls.h — ESPN URL builders
// Add new endpoint builders here. Depends only on types.h.
#include <string>
#include "../types.h"
using namespace std;

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

string buildNewsUrl(League league) {
    switch (league) {
        case NFL: return SITE_API + "/football/nfl/news";
        case NBA: return SITE_API + "/basketball/nba/news";
        default:  return "";
    }
}
