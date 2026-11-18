#pragma once
#include <string>

struct ProcessInfo;

class ProcessParser
{
public:
    bool parseStatLine(const std::string &statLine, ProcessInfo &process);
    bool parseVmRssLine(const std::string &statusLine, unsigned long long &rssKB);
};
