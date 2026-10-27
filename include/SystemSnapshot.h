#pragma once

struct SystemSnapshot
{
    double cpuPercent = 0.0;
    double memPercent = 0.0;
    unsigned long long usedMB = 0;
    unsigned long long totalMB = 0;
    unsigned long long rxKBs = 0;
    unsigned long long txKBs = 0;
};