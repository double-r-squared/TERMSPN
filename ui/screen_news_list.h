#pragma once
// ui/screen_news_list.h — scrollable news headline list
// Return values:
//   >= 0  article selected (index into articles vector)
//   -1    back to standings
//   -2    refresh requested (R key)
#include <ncurses.h>
#include <vector>
#include "../types.h"
#include "../utils/text.h"
#include "ui_core.h"
using namespace std;

int screenNewsList(const vector<NewsArticle>& articles) {
    int selected     = 0;
    int scrollOffset = 0;
    int n            = (int)articles.size();

    if (n == 0) {
        clear();
        drawTitleBar("ESPN Terminal  |  News");
        mvprintw(4, 4, "No articles available.");
        drawStatusBar("Q: back");
        refresh();
        int ch = getch();
        (void)ch;
        return -1;
    }

    // Each article occupies 3 display rows: headline, meta, blank separator
    const int ROWS_PER_ARTICLE = 3;

    while (true) {
        clear();
        int rows = getmaxy(stdscr);
        int cols = getmaxx(stdscr);
        int visibleRows = (rows - 4) / ROWS_PER_ARTICLE;

        drawTitleBar("ESPN Terminal  |  News");

        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(2, 1, "%-*s  %-8s  %s", cols - 20, "HEADLINE", "DATE", "TYPE");
        attroff(COLOR_PAIR(1) | A_BOLD);
        mvhline(3, 1, ACS_HLINE, cols - 2);

        int displayY = 4;

        for (int i = scrollOffset; i < n && displayY < rows - 2; i++) {
            const NewsArticle& a = articles[i];
            bool isSelected = (i == selected);

            // Truncate headline to fit terminal width minus right-side meta
            int maxHeadline = cols - 4;
            string hl = a.headline.size() > (size_t)maxHeadline
                        ? a.headline.substr(0, maxHeadline - 1) + "…"
                        : a.headline;

            // Row 1 — headline
            if (isSelected) attron(COLOR_PAIR(3) | A_BOLD);
            else             attron(COLOR_PAIR(2));
            mvprintw(displayY, 2, "%-*s", cols - 3, hl.c_str());
            if (isSelected) attroff(COLOR_PAIR(3) | A_BOLD);
            else             attroff(COLOR_PAIR(2));
            displayY++;

            // Row 2 — author · date · [type tag]
            if (displayY >= rows - 2) break;
            string author = a.author.empty() ? "ESPN" : a.author;
            string date   = formatDate(a.published);
            string tag    = (a.type == "Story") ? "[Story]" : "[News] ";
            string meta   = author + "  ·  " + date;

            attron(COLOR_PAIR(4));
            mvprintw(displayY, 4, "%s", meta.c_str());
            attroff(COLOR_PAIR(4));

            attron(COLOR_PAIR(1));
            mvprintw(displayY, cols - 10, "%s", tag.c_str());
            attroff(COLOR_PAIR(1));
            displayY++;

            // Row 3 — blank separator
            displayY++;
        }

        drawStatusBar("Up/Down: navigate  ENTER: read story  R: refresh  Q: back");
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
            case 'r':  case 'R':       return -2;
            case 'q':  case 'Q':       return -1;
        }
    }
}
