#include "MemCollector.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

bool parseMemInfo(std::istream &input, unsigned long long &total, unsigned long long &available)
{

    std::string key;
    unsigned long long value = 0;
    unsigned long long localTotal = 0;
    unsigned long long localAvailable = 0;
    bool foundTotal = false;
    bool foundAvailable = false;
    std::string line;

    while (std::getline(input, line))
    {
        std::istringstream ss(line);
        if (ss >> key >> value)
        {
            if (key == "MemTotal:")
            {
                localTotal = value;
                foundTotal = true;
            }
            else if (key == "MemAvailable:")
            {
                localAvailable = value;
                foundAvailable = true;
            }
        }
    }
    if (!foundTotal || !foundAvailable)
    {
        return false;
    }
    if (localTotal == 0 || localAvailable > localTotal)
    {
        return false;
    }

    total = localTotal;
    available = localAvailable;
    return true;
}

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
    unsigned long long total = 0;
    unsigned long long available = 0;
    if (!parseMemInfo(file, total, available))
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