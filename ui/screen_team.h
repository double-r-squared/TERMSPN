#pragma once
// ui/screen_team.h — team detail: schedule list (left) + team stats (right)
// Returns:
//   -1  back to standings (Q / ESC)
//   -2  refresh schedule requested (R)
#include <ncurses.h>
#include <vector>
#include <algorithm>
#include "../types.h"
#include "../utils/text.h"
#include "../utils/assetLoader.h"
#include "ui_core.h"
using namespace std;

// "D. Booker (31)" → "Booker (31)"
static string lastNameStat(const string& s) {
    auto paren = s.find(" (");
    if (paren == string::npos) return s;
    string name = s.substr(0, paren);
    auto sp = name.rfind(' ');
    return (sp != string::npos ? name.substr(sp + 1) : name) + s.substr(paren);
}

int screenTeamDetail(const Team& team, const vector<GameResult>& schedule, League league) {
    int n = (int)schedule.size();

    auto logo = loadTeamLogo(league, team.abbreviation); // load team logo

    const int W_DATE = 8;
    const int W_HA   = 3;
    const int W_OPP  = 6;
    const int W_RES  = 10;
    const int W_REC  = 16;

    const int W_TEAM = W_DATE + W_HA + W_OPP + W_RES + W_REC;

    const int W_PTS = 20;
    const int W_REB = 20;
    const int W_AST = 20;

    // Start scroll near most recent completed game
    int scrollOffset = 0;
    for (int i = 0; i < n; i++)
        if (schedule[i].completed) scrollOffset = max(0, i - 2);

    while (true) {
        clear();
        int rows = getmaxy(stdscr);
        int cols = getmaxx(stdscr);

        int leftWidth = max(45, cols * 3 / 5);
        int divX      = leftWidth;
        int rightX    = leftWidth + 2;
        int rc2       = rightX + 14;   // value column in right pane

        // Leader column X positions — used in both the header and game rows

        const int ROWS_PER_GAME = 2;
        int visibleGames = max(1, (rows - 6) / ROWS_PER_GAME);  // header now 2 rows
        int maxScroll    = max(0, n - visibleGames);
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;

        drawTitleBar("ESPN Terminal  |  " + team.displayName);

        // ── Vertical divider ──────────────────────────────────────────────────
        for (int y = 1; y < rows - 1; y++)
            mvaddch(y, divX, ACS_VLINE);

        // ── Right pane: team stats ─────────────────────────────────────────────
        int ry = 2;
        int rLineW = cols - rightX - 1;

        // Team Logo
        for (string line : logo) {
            mvprintw(ry++, rightX, "%s", line.c_str());
        }

        mvhline(ry++, rightX, ACS_HLINE, rLineW);

        // Name (ABV)  "Second in East"
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(ry++, rightX, "%s  (%s)", team.displayName.c_str(), team.abbreviation.c_str());
        attroff(COLOR_PAIR(1) | A_BOLD);
        attron(COLOR_PAIR(4));
        printw("   %s", team.standing.c_str());
        attroff(COLOR_PAIR(4));

        mvhline(ry++, rightX, ACS_HLINE, rLineW);

        auto rrow = [&](const string& label, const string& val) {
            mvprintw(ry,  rightX, "%-14s", label.c_str());
            attron(A_BOLD);
            mvprintw(ry++, rc2, "%s", val.c_str());
            attroff(A_BOLD);
        };

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(ry++, rightX, "Record");
        attroff(COLOR_PAIR(1) | A_BOLD);

        rrow("Overall",    team.record);
        rrow("Home",       team.home);
        rrow("Away",       team.away);
        rrow("Division",   team.divRecord);
        rrow("Conference", team.conf);
        rrow("Last 10",    team.lastTen);
        ry++;

        mvhline(ry++, rightX, ACS_HLINE, rLineW);

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(ry++, rightX, "Per Game Stats");
        attroff(COLOR_PAIR(1) | A_BOLD);

        rrow("PPG",     team.ppg);
        rrow("OPP PPG", team.oppPpg);
        rrow("Diff",    team.diff);

        attroff(COLOR_PAIR(5) | A_BOLD);
        attroff(COLOR_PAIR(6) | A_BOLD);

        mvhline(ry++, rightX, ACS_HLINE, rLineW);

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(ry++, rightX, "Franchise");
        attroff(COLOR_PAIR(1) | A_BOLD);

        rrow("Venue",     team.venue);

        attroff(COLOR_PAIR(5) | A_BOLD);
        attroff(COLOR_PAIR(6) | A_BOLD);

        // ── Left pane: schedule header (2 rows + divider) ────────────────────
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(2, 1, "%-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s",
            W_DATE, "DATE", W_HA, "", W_OPP, "OPP",
            W_RES, "RESULT", W_REC - 6, "REC", W_PTS, "Hi PNTS",
            W_REB, "Hi REB", W_AST, "Hi ASS");
        attroff(COLOR_PAIR(1) | A_BOLD);
        mvhline(4, 1, ACS_HLINE, divX - 1);

        if (n == 0) {
            attron(COLOR_PAIR(4));
            mvprintw(6, 3, "No schedule data.");
            attroff(COLOR_PAIR(4));
        }

        int displayY = 5;
        for (int i = scrollOffset; i < n && displayY < rows - 2; i++) {
            const GameResult& g = schedule[i];
            string date = formatDate(g.date);
            string ha   = (g.homeAway == "home") ? "vs " : "@  ";

            // Row 1 — date / h-a / opponent / result or tip-off / record
            if (g.completed) {
                attron(COLOR_PAIR(2));
                mvprintw(displayY, 1, "%-*s %-*s %-*s ", 
                    W_DATE,  date.c_str(), 
                    W_HA,    ha.c_str(), 
                    W_OPP,   g.opponent.c_str());
                attroff(COLOR_PAIR(2));

                attron((g.result == "W" ? COLOR_PAIR(5) : COLOR_PAIR(6)) | A_BOLD);
                printw("%s", g.result.c_str());
                attroff(A_BOLD | COLOR_PAIR(5) | COLOR_PAIR(6));

                attron(COLOR_PAIR(2));
                string score = " " + g.ourScore + "-" + g.oppScore; // add two more spaces?
                mvprintw(displayY, W_DATE + W_HA + W_OPP + 5, "%-*s %-*s", 
                    W_RES - 1, score.c_str(),
                    W_REC,   g.record.c_str()
                );
                attroff(COLOR_PAIR(2));

            } else {
                // IF THERE IS A LIVE GAME
                attron(COLOR_PAIR(2));
                mvprintw(displayY, 1, "%-*s %-*s %-*s %-*s",
                    W_DATE, date.c_str(), 
                    W_HA, ha.c_str(), 
                    W_OPP, g.opponent.c_str(), 
                    12, g.statusText.c_str());
                attroff(COLOR_PAIR(2));
            }

            // Same Row — leaders aligned to COL_PTS / COL_REB / COL_AST
            if (g.completed && g.highPts != "--") {
                attron(COLOR_PAIR(4));
                mvprintw(displayY, W_TEAM, "%-*s %-*s %-*s", 
                    W_PTS, lastNameStat(g.highPts).c_str(),
                    W_REB, lastNameStat(g.highReb).c_str(),
                    W_AST, lastNameStat(g.highAst).c_str());
                attroff(COLOR_PAIR(4));
            }

            displayY++;
        }

        drawStatusBar("Up/Down: scroll  R: refresh schedule  Q: back");
        refresh();

        switch (getch()) {
            case KEY_UP:   if (scrollOffset > 0)         scrollOffset--; break;
            case KEY_DOWN: if (scrollOffset < maxScroll) scrollOffset++; break;
            case 'r': case 'R': return -2;
            case 'q': case 'Q': case 27: return -1;
        }
    }
}
