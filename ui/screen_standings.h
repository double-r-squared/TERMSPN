#pragma once
// ui/screen_standings.h — standings table screen
// Returns selected team index, or -1 to go back.
#include <ncurses.h>
#include <vector>
#include "../types.h"
#include "ui_core.h"
#include "../utils/assetLoader.h"
using namespace std;

int screenStandings(const vector<Team>& teams, int leagueIndex) {
    int selected     = 0;
    int scrollOffset = 0;
    int n            = (int)teams.size();

    const int W_SEED = 4;
    const int W_ABB  = 5;
    const int W_NAME = 22;
    const int W_REC  = 8;
    const int W_PCT  = 6;
    const int W_PPG  = 6;
    const int W_OPP  = 6;
    const int W_DIFF = 8;
    const int W_STRK = 4;

    const int tableWidth = W_SEED+1 + W_ABB+1 + W_NAME+1 + W_REC+1
                             + W_PCT+1 + W_PPG+1 + W_OPP+1 + W_DIFF+1 + W_STRK+1;

    auto logo = loadLeagueLogo(leagueIndex);

    while (true) {
        clear();
        int rows = getmaxy(stdscr);
        int cols = getmaxx(stdscr);
        int visibleRows = rows - 6;

        drawTitleBar("ESPN Terminal  |  Standings");

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(2, 1, "%-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s",
            W_SEED, "SEED", W_ABB, "TEAM", W_NAME, "NAME",
            W_REC, "W-L", W_PCT, "PCT", W_PPG, "PPG",
            W_OPP, "OPP", W_DIFF - 1, "DIFF", W_STRK, "STRK");
        attroff(COLOR_PAIR(1) | A_BOLD);
        mvhline(3, 1, ACS_HLINE, cols - 2);

        int displayY = 4;
        int seed     = 1;
        string lastConf = "";
        bool TOP = true;

        for (int i = scrollOffset; i < n && displayY < rows - 1; i++) {
            const Team& t = teams[i];

            if (t.conference != lastConf) {
                if (displayY >= rows - 1) break;
                attron(COLOR_PAIR(4) | A_BOLD);

                string confLabel = t.conference + " ";
                int fillLen = tableWidth - (int)confLabel.size() - 3;
                if (fillLen < 0) fillLen = 0;
                string fill = "";
                for (int j = 0; j < fillLen; j++) fill += "═";

                if (TOP) {
                    mvprintw(displayY++, 1, "%s%s╗", confLabel.c_str(), fill.c_str());
                    TOP = false;
                } else {
                    mvprintw(displayY, 1, "%s%s╣", confLabel.c_str(), fill.c_str());
                    attroff(COLOR_PAIR(4) | A_BOLD);
                    printw(" %s", logo[i].c_str());
                    displayY++;
                }

                attroff(COLOR_PAIR(4) | A_BOLD);
                seed     = 1;
                lastConf = t.conference;
            }

            if (displayY >= rows - 1) break;

            bool isSelected = (i == selected);
            bool isWin      = !t.streak.empty() && t.streak[0] == 'W';
            bool isLoss     = !t.streak.empty() && t.streak[0] == 'L';

            if (isSelected) attron(COLOR_PAIR(3) | A_BOLD);
            else            attron(COLOR_PAIR(2));

            mvprintw(displayY, 1, "%-*d %-*s %-*s %-*s %-*s %-*s %-*s %-*s",
                W_SEED, seed,
                W_ABB,  t.abbreviation.c_str(),
                W_NAME, t.displayName.c_str(),
                W_REC,  t.record.c_str(),
                W_PCT,  t.pct.c_str(),
                W_PPG,  t.ppg.c_str(),
                W_OPP,  t.oppPpg.c_str(),
                W_DIFF, t.diff.c_str());

            int streakX = 1 + W_SEED+1 + W_ABB+1 + W_NAME+1 + W_REC+1
                            + W_PCT+1 + W_PPG+1 + W_OPP+1 + W_DIFF;

            if (!isSelected) {
                attroff(COLOR_PAIR(2));
                if (isWin)       attron(COLOR_PAIR(5) | A_BOLD);
                else if (isLoss) attron(COLOR_PAIR(6) | A_BOLD);
                else             attron(COLOR_PAIR(2));
            }
            mvprintw(displayY, streakX, "%-*s", W_STRK, t.streak.c_str());
            attroff(A_BOLD);
            attroff(COLOR_PAIR(2));
            attroff(COLOR_PAIR(3));
            attroff(COLOR_PAIR(5));
            attroff(COLOR_PAIR(6));

            attron(COLOR_PAIR(4) | A_BOLD);
            mvprintw(displayY, tableWidth - 2, "║ ");
            attroff(COLOR_PAIR(4) | A_BOLD);
            printw("%s", logo[i].c_str());

            displayY++;
            seed++;
        }

        string bottomFill = "";
        for (int j = 0; j < tableWidth - 3; j++) bottomFill += "═";
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw((rows - 2 > displayY) ? displayY : rows - 2, 1, "%s╝", bottomFill.c_str());
        attroff(COLOR_PAIR(4) | A_BOLD);

        drawStatusBar("Up/Down: navigate  ENTER: team detail  N: news  R: refresh  Q: back");
        refresh();

        switch (getch()) {
            case KEY_UP:
                if (selected > 0) {
                    selected--;
                    if (selected < scrollOffset) scrollOffset--;
                }
                break;
            case KEY_DOWN:
                if (selected < n - 1) {
                    selected++;
                    if (selected >= scrollOffset + visibleRows) scrollOffset++;
                }
                break;
            case '\n': case KEY_ENTER: return selected;
            case 'r': case 'R': return -2;
            case 'n': case 'N': return -3;
            case 'q': case 'Q': return -1;
        }
    }
}
