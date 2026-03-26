#pragma once
#include <ncurses.h>
#include <locale.h>
#include <string>
#include <vector>
#include "espn-api.h"
#include "assetLoader.h"
using namespace std;

// Color pairs:
// 1 = cyan         (headers, labels)
// 2 = white        (normal rows)
// 3 = black/cyan   (selected row)
// 4 = yellow       (accents, conf headers)
// 5 = green        (win streak)
// 6 = red          (loss streak)
// 7 = white/blue   (title/status bars)

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

// ─── Chrome ───────────────────────────────────────────────────────────────────

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

// ─── League select ────────────────────────────────────────────────────────────

// Returns selected league index, or -1 to quit
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

// ─── Standings ────────────────────────────────────────────────────────────────

// Returns selected team index, or -1 to go back
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

    // total display width: start col(1) + each field + spaces between + "| " suffix
    const int tableWidth = W_SEED+1 + W_ABB+1 + W_NAME+1 + W_REC+1
                             + W_PCT+1 + W_PPG+1 + W_OPP+1 + W_DIFF+1 + W_STRK+1;

    auto logo = loadLeagueLogo(leagueIndex);     // Logo Setup HERE

    while (true) {
        clear();
        int rows = getmaxy(stdscr);
        int cols = getmaxx(stdscr);
        int visibleRows = rows - 6;

        drawTitleBar("ESPN Terminal  |  Standings");

        // header
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

            // conference separator
            if (t.conference != lastConf) {
                if (displayY >= rows - 1) break;
                attron(COLOR_PAIR(4) | A_BOLD);

                string confLabel = t.conference + " ";
                // each "═" is 3 bytes but 1 display col, so build fill by repeating the sequence
                int fillLen = tableWidth - (int)confLabel.size() - 3; // MARK: EDITED HERE -3 for corner char + space
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

        // bottom border — dynamically sized to match table width
        string bottomFill = "";
        for (int j = 0; j < tableWidth - 3; j++) bottomFill += "═"; // MARK: EDITED HERE
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw((rows - 2 > displayY) ? displayY : rows - 2, 1, "%s╝", bottomFill.c_str());
        attroff(COLOR_PAIR(4) | A_BOLD);

        drawStatusBar("Up/Down: navigate  ENTER: team detail  Q: back");
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
            case 'q': case 'Q': return -1;
        }
    }
}

// ─── Team detail ──────────────────────────────────────────────────────────────

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