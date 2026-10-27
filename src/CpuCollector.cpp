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

void CpuCollector::update(SystemSnapshot& snapshot)
{
    std::ifstream file("/proc/stat");
    std::string line;

    if (!file.is_open())
    {
        return;
    }
    std::getline(file, line);

    std::istringstream ss(line);

    std::string cpuLabel;
    ss >> cpuLabel;
    unsigned long long val = 0;
    unsigned long long total = 0;
    unsigned long long idle = 0;
    int index = 0;
    while (ss >> val)
    {
        total += val;
        if (index == 3)
        {
            idle = val;
        }
        ++index;
    }
    if(prevTotalTicks_ > 0){
        unsigned long long delta_total = total - prevTotalTicks_;
        unsigned long long delta_idle = idle -prevIdleTicks_;
        if(delta_total > 0 ){
            cpuPercent_ = 100 * (1.0 - static_cast<double>(delta_idle) / delta_total);
            snapshot.cpuPercent = cpuPercent_;
        }
    }
    prevTotalTicks_ = total;
    prevIdleTicks_ = idle;
}