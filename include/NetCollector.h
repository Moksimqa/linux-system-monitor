#pragma once
#include "IMetricCollector.h"
#include <chrono>
struct SystemSnapshot;

class NetCollector : public IMetricCollector
{
public:
    void update(SystemSnapshot &snapshot) override;
    std::string getName() const override;
    double getRxKBs() const;
    double getTxKBs() const;

private:
    unsigned long long prevRx_ = 0;
    unsigned long long prevTx_ = 0;
    double rxSpeedKBs_ = 0.0;
    double txSpeedKBs_ = 0.0;
    bool hasPrev_ = false;
    std::chrono::steady_clock::time_point prevTime_;
};