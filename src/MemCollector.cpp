#include "MemCollector.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

std::string MemCollector::getName() const
{
    return "RAM";
}

unsigned long long MemCollector::getTotalMB() const
{
    return memTotalKB_ / 1024;
}

unsigned long long MemCollector::getUsedMB() const
{
    return (memTotalKB_ - memAvailableKB_) / 1024;
}

double MemCollector::getMemPercent() const
{
    return memPercent_;
}

void MemCollector::update(SystemSnapshot &snapshot)
{
    std::ifstream file("/proc/meminfo");
    if (!file.is_open())
    {
        return;
    }
    std::string key;
    unsigned long long value = 0;
    unsigned long long total = 0;
    unsigned long long available = 0;
    std::string line;

    while ((total == 0 || available == 0) && std::getline(file, line))
    {
        std::istringstream ss(line);
        if (ss >> key >> value)
        {
            if (key == "MemTotal:")
            {
                total = value;
            }
            else if (key == "MemAvailable:")
            {
                available = value;
            }
        }
    }
    if (total == 0 || available == 0)
    {
        return;
    }
    memTotalKB_ = total;
    memAvailableKB_ = available;

    memPercent_ = 100 * (1.0 - static_cast<double>(memAvailableKB_) / memTotalKB_);
    snapshot.totalMB = getTotalMB();
    snapshot.usedMB = getUsedMB();
    snapshot.memPercent = getMemPercent();
}