#pragma once
// http.h — raw HTTP fetch utility
// No ESPN-specific knowledge. Depends on nothing local.
#include <string>
#include <curl/curl.h>
using namespace std;

static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    static_cast<string*>(userp)->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

string fetchUrl(const string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";
    string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) return "";
    return response;
}
