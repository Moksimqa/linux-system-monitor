#include "NetCollector.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string NetCollector::getName() const
{
    return "NET";
}

unsigned long long NetCollector::getRxKBs() const
{
    return rxSpeedKBs_;
}

unsigned long long NetCollector::getTxKBs() const
{
    return txSpeedKBs_;
}

void NetCollector::update(SystemSnapshot& snapshot)
{
    std::ifstream file("/proc/net/dev");
    std::string line;

    if (!file.is_open())
    {
        return;
    }

    std::getline(file, line);
    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string iface;
        ss >> iface;

        if (iface == "lo:")
        {
            continue;
        }

        unsigned long long rxBytes;
        ss >> rxBytes;
        unsigned long long dummy;
        for (int i = 0; i < 7; ++i)
        {
            ss >> dummy;
        }
        unsigned long long txBytes;
        ss >> txBytes;

        if (prevRx_ > 0)
        {
            rxSpeedKBs_ = (rxBytes - prevRx_) / 1024;
            txSpeedKBs_ = (txBytes - prevTx_) / 1024;
            snapshot.rxKBs = rxSpeedKBs_;
            snapshot.txKBs = txSpeedKBs_;
        }
        prevRx_ = rxBytes;
        prevTx_ = txBytes;
        break;
    }
}