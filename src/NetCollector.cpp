#include "NetCollector.h"
#include "SystemSnapshot.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <string>

bool parseNetDev(std::istream &input, std::unordered_map<std::string, NetInterfaceCounters> &interfaces)
{
    std::unordered_map<std::string, NetInterfaceCounters> localInterfaces;

    std::string line;

    if (!std::getline(input, line))
    {
        return false;
    }

    if (!std::getline(input, line))
    {
        return false;
    }

    while (std::getline(input, line))
    {
        std::istringstream ss(line);
        std::string iface;

        if (!(ss >> iface))
        {
            return false;
        }

        if (iface.empty() || iface.back() != ':')
        {
            return false;
        }

        iface.pop_back();

        if (iface == "lo")
        {
            continue;
        }
        unsigned long long rxBytes = 0;
        if (!(ss >> rxBytes))
        {
            return false;
        }

        unsigned long long dummy = 0;
        for (int i = 0; i < 7; ++i)
        {
            if (!(ss >> dummy))
            {
                return false;
            }
        }
        unsigned long long txBytes = 0;
        if (!(ss >> txBytes))
        {
            return false;
        }
        localInterfaces[iface] = {rxBytes, txBytes};
    }

    interfaces = localInterfaces;
    return true;
}

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
    std::unordered_map<std::string, NetInterfaceCounters> currentInterfaces;
    if (!parseNetDev(file, currentInterfaces))
    {
        return;
    }
    auto now = std::chrono::steady_clock::now();

    unsigned long long deltaRxBytes = 0;
    unsigned long long deltaTxBytes = 0;
    if (hasPrev_)
    {
        for (const auto &[iface, current] : currentInterfaces)
        {
            auto it = prevInterfaces_.find(iface);

            if (it == prevInterfaces_.end())
            {
                continue;
            }
            const NetInterfaceCounters &previous = it->second;
            if (current.rxBytes >= previous.rxBytes)
            {
                deltaRxBytes += current.rxBytes - previous.rxBytes;
            }

            if (current.txBytes >= previous.txBytes)
            {
                deltaTxBytes += current.txBytes - previous.txBytes;
            }
        }
        std::chrono::duration<double> elapsed = now - prevTime_;
        double dt = elapsed.count();

        if (dt > 0.0)
        {
            rxSpeedKBs_ = deltaRxBytes / 1024.0 / dt;
            txSpeedKBs_ = deltaTxBytes / 1024.0 / dt;

            snapshot.rxKBs = rxSpeedKBs_;
            snapshot.txKBs = txSpeedKBs_;
        }
    }
    prevInterfaces_ = currentInterfaces;
    prevTime_ = now;
    hasPrev_ = true;
}