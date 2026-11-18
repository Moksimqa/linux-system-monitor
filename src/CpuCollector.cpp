#include "CpuCollector.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool parseCpuStatLine(const std::string &line, unsigned long long &total, unsigned long long &idle)
{
    unsigned long long localTotal = 0;
    unsigned long long localIdle = 0;

    std::istringstream ss(line);
    std::string cpuLabel;
    if (!(ss >> cpuLabel) || cpuLabel != "cpu")
    {
        return false;
    }
    for (int index = 0; index < 8; ++index)
    {
        unsigned long long value = 0;
        if (!(ss >> value))
        {
            return false;
        }
        localTotal += value;

        if (index == 3 || index == 4)
        {
            localIdle += value;
        }
    }
    total = localTotal;
    idle = localIdle;
    return true;
}

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

    unsigned long long total = 0;
    unsigned long long idle = 0;

    if (!parseCpuStatLine(line, total, idle))
    {
        return;
    }

    if (hasPrev_ && total > prevTotalTicks_ && idle >= prevIdleTicks_)
    {
        unsigned long long delta_total = total - prevTotalTicks_;
        unsigned long long delta_idle = idle - prevIdleTicks_;
        if (delta_idle <= delta_total)
        {
            cpuPercent_ = 100 * (1.0 - static_cast<double>(delta_idle) / delta_total);
            snapshot.cpuPercent = cpuPercent_;
        }
    }
    prevTotalTicks_ = total;
    prevIdleTicks_ = idle;
    hasPrev_ = true;
}