#pragma once

struct SystemSnapshot
{
    double cpuPercent = 0.0;
    double memPercent = 0.0;
    unsigned long long usedMB = 0;
    unsigned long long totalMB = 0;
    double rxKBs = 0;
    double txKBs = 0;
};