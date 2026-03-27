#pragma once
// utils/text.h — plain-text rendering helpers for terminal output
// No ncurses or ESPN knowledge. Safe to include anywhere.
#include <string>
#include <vector>
#include <sstream>
using namespace std;

// Strip HTML tags from a string, converting <p>/<br> to newlines.
// nlohmann::json already decodes \uXXXX escapes, so only &entity; refs remain.
static string stripHtml(const string& html) {
    string stripped;
    bool inTag = false;
    string tag;

    for (size_t i = 0; i < html.size(); i++) {
        char c = html[i];
        if (c == '<') {
            inTag = true;
            tag = "";
        } else if (c == '>') {
            if (tag == "/p" || tag == "p" || tag == "br" || tag == "br/")
                stripped += '\n';
            inTag = false;
            tag = "";
        } else if (inTag) {
            tag += (char)tolower(c);
        } else {
            stripped += c;
        }
    }

    // Decode common HTML entities
    string result;
    for (size_t i = 0; i < stripped.size(); i++) {
        if (stripped[i] == '&') {
            size_t semi = stripped.find(';', i);
            if (semi != string::npos) {
                string ent = stripped.substr(i + 1, semi - i - 1);
                if      (ent == "amp")  { result += '&';  i = semi; }
                else if (ent == "lt")   { result += '<';  i = semi; }
                else if (ent == "gt")   { result += '>';  i = semi; }
                else if (ent == "quot") { result += '"';  i = semi; }
                else if (ent == "nbsp") { result += ' ';  i = semi; }
                else                    { result += '&'; }
            } else {
                result += '&';
            }
        } else {
            result += stripped[i];
        }
    }
    return result;
}

// Word-wrap a string to fit within `width` columns.
// Preserves explicit newlines already in the text.
static vector<string> wordWrap(const string& text, int width) {
    vector<string> lines;
    istringstream paragraphs(text);
    string para;
    while (getline(paragraphs, para)) {
        if (para.empty()) { lines.push_back(""); continue; }
        istringstream words(para);
        string word, line;
        while (words >> word) {
            if (!line.empty() && (int)(line.size() + 1 + word.size()) > width) {
                lines.push_back(line);
                line = word;
            } else {
                if (!line.empty()) line += ' ';
                line += word;
            }
        }
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

// Format ISO 8601 date string to "Mar 26" for display.
static string formatDate(const string& iso) {
    if (iso.size() < 10) return iso;
    static const char* months[] = {
        "Jan","Feb","Mar","Apr","May","Jun",
        "Jul","Aug","Sep","Oct","Nov","Dec"
    };
    int month = stoi(iso.substr(5, 2)) - 1;
    int day   = stoi(iso.substr(8, 2));
    if (month < 0 || month > 11) return iso.substr(0, 10);
    return string(months[month]) + " " + to_string(day);
}
