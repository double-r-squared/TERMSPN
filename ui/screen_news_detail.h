#pragma once
// ui/screen_news_detail.h — full story reader with scrollable body
// Exits on Q or ESC, returning to the news list.
#include <ncurses.h>
#include <vector>
#include "../types.h"
#include "../utils/text.h"
#include "ui_core.h"
using namespace std;

void screenNewsDetail(const NewsArticle& article) {
    int scrollOffset = 0;

    while (true) {
        clear();
        int rows = getmaxy(stdscr);
        int cols = getmaxx(stdscr);
        int bodyWidth = cols - 6;

        // Title bar: truncated headline
        string title = article.headline.size() > (size_t)(cols - 4)
                       ? article.headline.substr(0, cols - 7) + "…"
                       : article.headline;
        drawTitleBar(title);

        int y = 2;

        // Author + date line
        string author = article.author.empty() ? "ESPN" : article.author;
        string date   = formatDate(article.published);
        attron(COLOR_PAIR(4));
        mvprintw(y++, 3, "%s  ·  %s", author.c_str(), date.c_str());
        attroff(COLOR_PAIR(4));

        // Section / league tags from teams and athletes
        if (!article.teams.empty() || !article.athletes.empty()) {
            string tags;
            for (auto& t : article.teams)    { if (!tags.empty()) tags += "  "; tags += t; }
            for (auto& a : article.athletes) { if (!tags.empty()) tags += "  "; tags += a; }
            attron(COLOR_PAIR(1));
            mvprintw(y++, 3, "%s", tags.substr(0, cols - 6).c_str());
            attroff(COLOR_PAIR(1));
        }

        mvhline(y++, 2, ACS_HLINE, cols - 4);

        // Body: use story if enriched, otherwise fall back to description
        const string& bodyText = article.story.empty() ? article.description : article.story;
        vector<string> lines = wordWrap(bodyText, bodyWidth);

        int bodyStart = y;
        int visibleLines = rows - bodyStart - 2;

        // Clamp scroll
        int maxScroll = max(0, (int)lines.size() - visibleLines);
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;

        for (int i = scrollOffset; i < (int)lines.size() && y < rows - 1; i++, y++) {
            mvprintw(y, 3, "%s", lines[i].c_str());
        }

        // Scroll indicator if content overflows
        if ((int)lines.size() > visibleLines) {
            attron(COLOR_PAIR(4));
            mvprintw(rows - 2, cols - 16, "[%d/%d lines]",
                     scrollOffset + visibleLines, (int)lines.size());
            attroff(COLOR_PAIR(4));
        }

        drawStatusBar("Up/Down: scroll  Q / ESC: back to news");
        refresh();

        switch (getch()) {
            case KEY_UP:
                if (scrollOffset > 0) scrollOffset--;
                break;
            case KEY_DOWN:
                if (scrollOffset < maxScroll) scrollOffset++;
                break;
            case 'q': case 'Q': case 27: return;
        }
    }
}
