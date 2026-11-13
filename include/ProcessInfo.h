#pragma once
#include <string>

struct ProcessInfo
{
    int pid = 0;
    std::string name;
    char state = '?';
    unsigned long long rssKB = 0;
};