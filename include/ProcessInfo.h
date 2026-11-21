#pragma once
#include <string>

struct ProcessInfo
{
    int pid = 0;
    std::string name;
    char state = '?';
    unsigned long long rssKB = 0;
    unsigned long long utime = 0;
    unsigned long long stime = 0;
    unsigned long long starttime = 0;
    double cpuPercent = 0.0;
};