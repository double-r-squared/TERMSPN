#pragma once
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const string ASSET_DIR = "./assets/";

vector<string> loadLogo(string &logoName) {
    vector<string> logo;

    ifstream file(ASSET_DIR + logoName, ios::binary); // Open in binary mode to preserve escape codes
    if (!file.is_open()) {
        cerr << "Error: Could not open the file!" << endl;
        return logo;
    }
    string line;
    while (getline(file, line)) {  // until EOF terminator = 0
        logo.push_back(line);
    }
    file.close();
    return logo;
}