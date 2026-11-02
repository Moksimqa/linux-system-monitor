#include "NetCollector.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

std::string NetCollector::getName() const
{
    return "NET";
}

double NetCollector::getRxKBs() const
{
    return rxSpeedKBs_;
}

double NetCollector::getTxKBs() const
{
    return txSpeedKBs_;
}

void NetCollector::update(SystemSnapshot &snapshot)
{
    std::ifstream file("/proc/net/dev");

    if (!file.is_open())
    {
        return;
    }
    std::string line;
    unsigned long long currentRxBytes = 0;
    unsigned long long currentTxBytes = 0;
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

        unsigned long long rxBytes = 0;
        ss >> rxBytes;
        currentRxBytes += rxBytes;
        unsigned long long dummy = 0;
        for (int i = 0; i < 7; ++i)
        {
            ss >> dummy;
        }
        unsigned long long txBytes = 0;
        ss >> txBytes;
        currentTxBytes += txBytes;
    }
    auto now = std::chrono::steady_clock::now();
    if (hasPrev_ && currentRxBytes >= prevRx_ && currentTxBytes >= prevTx_)
    {
        std::chrono::duration<double> elapsed = now - prevTime_;
        double dt = elapsed.count();
        if (dt > 0)
        {
            rxSpeedKBs_ = (currentRxBytes - prevRx_) / 1024.0 / dt;
            txSpeedKBs_ = (currentTxBytes - prevTx_) / 1024.0 / dt;
        }
        snapshot.rxKBs = rxSpeedKBs_;
        snapshot.txKBs = txSpeedKBs_;
    }
    prevTime_ = now;
    prevRx_ = currentRxBytes;
    prevTx_ = currentTxBytes;
    hasPrev_ = true;
}