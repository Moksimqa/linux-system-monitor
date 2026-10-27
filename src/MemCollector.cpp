#include "MemCollector.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <fstream>

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

void MemCollector::update(SystemSnapshot& snapshot)
{
    std::ifstream file("/proc/meminfo");
    if (!file.is_open())
    {
        return;
    }
    std::string key;
    std::string unit;
    unsigned long long value = 0;
    while (file >> key >> value >> unit)
    {
        if (key == "MemTotal:")
        {
            memTotalKB_ = value;
           
        }
        else if (key == "MemAvailable:")
        {
            memAvailableKB_ = value;

        }
        if (memTotalKB_ > 0 && memAvailableKB_ > 0)
        {
            break;
        }
    }

    if (memTotalKB_ > 0)
    {
        memPercent_ = 100 * (1.0 - static_cast<double>(memAvailableKB_) / memTotalKB_);
        snapshot.totalMB = getTotalMB();
        snapshot.usedMB = getUsedMB();
        snapshot.memPercent = getMemPercent();
    }
}