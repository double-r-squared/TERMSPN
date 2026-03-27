#pragma once
// ui/screen_league.h — league selection screen
// Returns selected league index, or -1 to quit.
#include <ncurses.h>
#include "../types.h"
#include "ui_core.h"
using namespace std;

int screenLeagueSelect() {
    int selected = 0;
    int n = (int)LEAGUE_COUNT;

    while (true) {
        clear();
        drawTitleBar("ESPN Terminal  |  League Select");

        int rows = getmaxy(stdscr);
        int cols = getmaxx(stdscr);
        int startY = rows / 2 - n;
        int startX = cols / 2 - 10;

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(startY - 2, startX, "Select a League");
        attroff(COLOR_PAIR(1) | A_BOLD);

        for (int i = 0; i < n; i++) {
            if (i == selected) {
                attron(COLOR_PAIR(3) | A_BOLD);
                mvprintw(startY + i * 2, startX - 2, " > %-16s ", LEAGUE_NAMES[i].c_str());
                attroff(COLOR_PAIR(3) | A_BOLD);
            } else {
                attron(COLOR_PAIR(2));
                mvprintw(startY + i * 2, startX - 2, "   %-16s ", LEAGUE_NAMES[i].c_str());
                attroff(COLOR_PAIR(2));
            }
        }

        drawStatusBar("Up/Down: move  ENTER: select  Q: quit");
        refresh();

        switch (getch()) {
            case KEY_UP:   selected = (selected - 1 + n) % n; break;
            case KEY_DOWN: selected = (selected + 1) % n;     break;
            case '\n': case KEY_ENTER: return selected;
            case 'q': case 'Q': return -1;
        }
    }
}
