#pragma once
// ui/ui_core.h — ncurses lifecycle and shared chrome (title bar, status bar)
// Include this in every screen. All screens depend on this.
//
// Color pairs:
// 1 = cyan         (headers, labels)
// 2 = white        (normal rows)
// 3 = black/cyan   (selected row)
// 4 = yellow       (accents, conf headers)
// 5 = green        (win streak)
// 6 = red          (loss streak)
// 7 = white/blue   (title/status bars)
#include <ncurses.h>
#include <locale.h>
#include <string>
using namespace std;

void uiInit() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();
    use_default_colors();
    init_pair(1, COLOR_CYAN,   -1);
    init_pair(2, COLOR_WHITE,  -1);
    init_pair(3, COLOR_BLACK,  COLOR_CYAN);
    init_pair(4, COLOR_YELLOW, -1);
    init_pair(5, COLOR_GREEN,  -1);
    init_pair(6, COLOR_RED,    -1);
    init_pair(7, COLOR_WHITE,  COLOR_BLUE);
}

void uiShutdown() { endwin(); }

void drawTitleBar(const string& title) {
    int cols = getmaxx(stdscr);
    attron(COLOR_PAIR(7) | A_BOLD);
    mvhline(0, 0, ' ', cols);
    mvprintw(0, 2, "%s", title.c_str());
    attroff(COLOR_PAIR(7) | A_BOLD);
}

void drawStatusBar(const string& msg) {
    int rows = getmaxy(stdscr);
    int cols = getmaxx(stdscr);
    string m = msg.substr(0, max(0, cols - 4));
    attron(COLOR_PAIR(7));
    mvhline(rows - 1, 0, ' ', cols);
    mvprintw(rows - 1, 2, "%s", m.c_str());
    attroff(COLOR_PAIR(7));
}
