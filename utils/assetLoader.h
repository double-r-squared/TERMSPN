#pragma once
// utils/assetLoader.h — loads ASCII art logo files from ./assets/
// Moved from: assetLoader.h (root)
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

const string ASSET_DIR = "./assets/";

vector<string> loadLeagueLogo(int leagueIndex) {
    string logoName;

    switch (leagueIndex) {
        case 0: logoName  = "NBA_LOGO.txt"; break;
        case 1: logoName  = "NFL_LOGO.txt"; break;
        default: logoName = "";
    }

    vector<string> logo;
    ifstream file(ASSET_DIR + logoName, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Could not open the file!" << endl;
        return logo;
    }
    string line;
    while (getline(file, line)) {
        logo.push_back(line);
    }
    file.close();
    return logo;
}

// Search by Abriviation
vector<string> loadTeamLogo(int leagueIndex, string abv) {
    string logoPath = "";

    switch (leagueIndex) {
        case 0: logoPath  = ASSET_DIR + "NBA/"; break;
        case 1: logoPath  = ASSET_DIR + "NFL/"; break;
        default: logoPath = "";
    }

    vector<string> logo;
    ifstream file(logoPath + abv + ".txt", ios::binary); // PHX.txt
    if (!file.is_open()) {
        cerr << "Error: Could not open the file!" << endl;
        return logo;
    }
    string line;
    while (getline(file, line)) {
        logo.push_back(line);
    }
    file.close();
    return logo;
}
