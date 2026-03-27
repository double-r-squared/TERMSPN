#pragma once
// ui/screen_team.h — team detail screen
// Exits on Q, ESC, or q — returns to standings.
#include <ncurses.h>
#include "../types.h"
#include "ui_core.h"
using namespace std;

void screenTeamDetail(const Team& team) {
    while (true) {
        clear();
        drawTitleBar("ESPN Terminal  |  " + team.displayName);

        int col1 = 4, col2 = 28;
        int y = 3;

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(y++, col1, "%s  (%s)", team.displayName.c_str(), team.abbreviation.c_str());
        attroff(COLOR_PAIR(1) | A_BOLD);

        attron(COLOR_PAIR(4));
        mvprintw(y++, col1, "%s", team.standing.c_str());
        attroff(COLOR_PAIR(4));
        y++;

        mvhline(y++, col1, ACS_HLINE, 38);

        auto row = [&](const string& label, const string& value) {
            mvprintw(y, col1, "%-16s", label.c_str());
            attron(A_BOLD);
            mvprintw(y++, col2, "%s", value.c_str());
            attroff(A_BOLD);
        };

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(y++, col1, "Record");
        attroff(COLOR_PAIR(1) | A_BOLD);

        row("Overall",    team.record);
        row("Home",       team.home);
        row("Away",       team.away);
        row("Division",   team.divRecord);
        row("Conference", team.conf);
        row("Last 10",    team.lastTen);
        y++;

        mvhline(y++, col1, ACS_HLINE, 38);

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(y++, col1, "Per Game Stats");
        attroff(COLOR_PAIR(1) | A_BOLD);

        row("PPG",     team.ppg);
        row("OPP PPG", team.oppPpg);
        row("Diff",    team.diff);

        bool isWin = !team.streak.empty() && team.streak[0] == 'W';
        mvprintw(y, col1, "%-16s", "Streak");
        attron((isWin ? COLOR_PAIR(5) : COLOR_PAIR(6)) | A_BOLD);
        mvprintw(y, col2, "%s", team.streak.c_str());
        attroff(COLOR_PAIR(5) | A_BOLD);
        attroff(COLOR_PAIR(6) | A_BOLD);

        drawStatusBar("Q / ESC: back to standings");
        refresh();

        int ch = getch();
        if (ch == 'q' || ch == 'Q' || ch == 27) return;
    }
}
