#pragma once
#include "IMetricCollector.h"
#include <chrono>
#include <unordered_map>
#include <string>
#include <istream>

struct SystemSnapshot;

struct NetInterfaceCounters
{
    unsigned long long rxBytes = 0;
    unsigned long long txBytes = 0;
};

class NetCollector : public IMetricCollector
{
public:
    void update(SystemSnapshot &snapshot) override;
    std::string getName() const override;
    double getRxKBs() const;
    double getTxKBs() const;

private:
    std::unordered_map<std::string, NetInterfaceCounters> prevInterfaces_;
    double rxSpeedKBs_ = 0.0;
    double txSpeedKBs_ = 0.0;
    bool hasPrev_ = false;
    std::chrono::steady_clock::time_point prevTime_;
};

bool parseNetDev(std::istream &input, std::unordered_map<std::string, NetInterfaceCounters> &interfaces);