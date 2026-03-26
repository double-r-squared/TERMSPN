#include "espn-api.h"
#include "ui.h"

int main() {
    uiInit();

    enum State { LEAGUE_SELECT, STANDINGS, TEAM_DETAIL };
    State state = LEAGUE_SELECT;

    League selectedLeague = NBA;
    vector<Team> teams;
    int selectedTeam = 0;

    while (true) {
        switch (state) {

            case LEAGUE_SELECT: {
                int result = screenLeagueSelect();
                if (result == -1) goto done;
                selectedLeague = (League)result;
                drawTitleBar("Fetching standings...");
                refresh();
                teams = parseStandings(fetchUrl(buildStandingsUrl(selectedLeague)));
                if (!teams.empty()) state = STANDINGS;
                break;
            }

            case STANDINGS: {
                int result = screenStandings(teams);
                if (result == -1) { state = LEAGUE_SELECT; break; }
                selectedTeam = result;
                // enrich with standing summary if not yet fetched
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
        }
    }

done:
    uiShutdown();
    return 0;
}