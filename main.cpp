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
// In-memory only. Both caches are discarded automatically on exit.
// standings: keyed by League, populated on first league visit.
// news:      keyed by League, populated alongside standings.
//            Individual article story bodies are lazy-fetched and stored in the
//            local `articles` vector; the cache holds the un-enriched list.

static map<League, vector<Team>>        standingsCache;
static map<League, vector<NewsArticle>> newsCache;

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

static void evictStandings(League league) { standingsCache.erase(league); }
static void evictNews(League league)      { newsCache.erase(league); }

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    uiInit();

    enum State { LEAGUE_SELECT, STANDINGS, TEAM_DETAIL, NEWS_LIST, NEWS_DETAIL };
    State state = LEAGUE_SELECT;

    League selectedLeague  = NBA;
    vector<Team>        teams;
    vector<NewsArticle> articles;
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
                state = TEAM_DETAIL;
                break;
            }

            case TEAM_DETAIL: {
                screenTeamDetail(teams[selectedTeam]);
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
                // Lazy-fetch story body; stored in local articles vector for this session
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
