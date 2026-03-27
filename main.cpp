#include <map>
#include "types.h"
#include "http.h"
#include "api/urls.h"
#include "api/parse.h"
#include "ui/ui_core.h"
#include "ui/screen_league.h"
#include "ui/screen_standings.h"
#include "ui/screen_team.h"
#include "ui/screen_news_list.h"
#include "ui/screen_news_detail.h"

// ─── Session cache ────────────────────────────────────────────────────────────
// All caches are in-memory only — discarded automatically on exit, no disk I/O.
//
// standingsCache  keyed by League       — populated on league select
// newsCache       keyed by League       — populated alongside standings
// scheduleCache   keyed by team ID str  — populated lazily on first team visit
//
// Individual article story bodies are lazy-fetched into the local `articles`
// vector; the newsCache holds the un-enriched list.

static map<League, vector<Team>>        standingsCache;
static map<League, vector<NewsArticle>> newsCache;
static map<string, vector<GameResult>>  scheduleCache;

static vector<Team> getStandings(League league) {
    if (standingsCache.count(league)) return standingsCache[league];
    vector<Team> teams = parseStandings(fetchUrl(buildStandingsUrl(league)), league);
    if (!teams.empty()) standingsCache[league] = teams;
    return teams;
}

static vector<NewsArticle> getNews(League league) {
    if (newsCache.count(league)) return newsCache[league];
    vector<NewsArticle> articles = parseNewsList(fetchUrl(buildNewsUrl(league)));
    if (!articles.empty()) newsCache[league] = articles;
    return articles;
}

static vector<GameResult> getSchedule(League league, const string& teamId) {
    if (scheduleCache.count(teamId)) return scheduleCache[teamId];
    vector<GameResult> games = parseSchedule(fetchUrl(buildTeamScheduleURL(league, teamId)));
    if (!games.empty()) scheduleCache[teamId] = games;
    return games;
}

static void evictStandings(League league)        { standingsCache.erase(league); }
static void evictNews(League league)             { newsCache.erase(league); }
static void evictSchedule(const string& teamId)  { scheduleCache.erase(teamId); }

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    uiInit();

    enum State { LEAGUE_SELECT, STANDINGS, TEAM_DETAIL, NEWS_LIST, NEWS_DETAIL };
    State state = LEAGUE_SELECT;

    League selectedLeague  = NBA;
    vector<Team>        teams;
    vector<NewsArticle> articles;
    vector<GameResult>  schedule;
    int selectedTeam    = 0;
    int selectedArticle = 0;
    bool running = true;

    while (running) {
        switch (state) {

            case LEAGUE_SELECT: {
                int result = screenLeagueSelect();
                if (result == -1) { running = false; break; }
                selectedLeague = (League)result;

                drawTitleBar("Fetching standings...");
                refresh();
                teams = getStandings(selectedLeague);

                drawTitleBar("Fetching news...");
                refresh();
                articles = getNews(selectedLeague);

                if (!teams.empty()) state = STANDINGS;
                break;
            }

            case STANDINGS: {
                int result = screenStandings(teams, selectedLeague);
                if (result == -1) { state = LEAGUE_SELECT; break; }
                if (result == -2) {
                    evictStandings(selectedLeague);
                    drawTitleBar("Refreshing standings...");
                    refresh();
                    teams = getStandings(selectedLeague);
                    break;
                }
                if (result == -3) { state = NEWS_LIST; break; }

                selectedTeam = result;
                if (teams[selectedTeam].standing.empty())
                    enrichTeamDetail(teams[selectedTeam], selectedLeague);

                drawTitleBar("Fetching schedule...");
                refresh();
                schedule = getSchedule(selectedLeague, teams[selectedTeam].id);

                state = TEAM_DETAIL;
                break;
            }

            case TEAM_DETAIL: {
                int result = screenTeamDetail(teams[selectedTeam], schedule);
                if (result == -2) {
                    evictSchedule(teams[selectedTeam].id);
                    drawTitleBar("Refreshing schedule...");
                    refresh();
                    schedule = getSchedule(selectedLeague, teams[selectedTeam].id);
                    break;  // re-enter TEAM_DETAIL with fresh data
                }
                state = STANDINGS;
                break;
            }

            case NEWS_LIST: {
                int result = screenNewsList(articles);
                if (result == -1) { state = STANDINGS; break; }
                if (result == -2) {
                    evictNews(selectedLeague);
                    drawTitleBar("Refreshing news...");
                    refresh();
                    articles = getNews(selectedLeague);
                    break;
                }
                selectedArticle = result;
                if (articles[selectedArticle].story.empty())
                    enrichNewsStory(articles[selectedArticle]);
                state = NEWS_DETAIL;
                break;
            }

            case NEWS_DETAIL: {
                screenNewsDetail(articles[selectedArticle]);
                state = NEWS_LIST;
                break;
            }
        }
    }

    uiShutdown();
    return 0;
}
