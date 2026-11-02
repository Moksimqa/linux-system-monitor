#include "CpuCollector.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string CpuCollector::getName() const
{
    return "CPU";
}

double CpuCollector::getCpuPercent() const
{
    return cpuPercent_;
}

void CpuCollector::update(SystemSnapshot &snapshot)
{
    std::ifstream file("/proc/stat");

    if (!file.is_open())
    {
        return;
    }

    std::string line;
    if (!std::getline(file, line))
    {
        return;
    }

    std::istringstream ss(line);
    std::string cpuLabel;
    if (!(ss >> cpuLabel) || cpuLabel != "cpu")
    {
        return;
    }
    unsigned long long val = 0;
    unsigned long long total = 0;
    unsigned long long idle = 0;
    int index = 0;
    while (ss >> val)
    {
        if (index < 8)
        {
            total += val;
        }
        if (index == 3 || index == 4)
        {
            idle += val;
        }
        ++index;
    }
    if (hasPrev_ && total > prevTotalTicks_ && idle >= prevIdleTicks_)
    {
        unsigned long long delta_total = total - prevTotalTicks_;
        unsigned long long delta_idle = idle - prevIdleTicks_;
        if (delta_total > 0)
        {
            cpuPercent_ = 100 * (1.0 - static_cast<double>(delta_idle) / delta_total);
            snapshot.cpuPercent = cpuPercent_;
        }
    }
    prevTotalTicks_ = total;
    prevIdleTicks_ = idle;
    hasPrev_ = true;
}