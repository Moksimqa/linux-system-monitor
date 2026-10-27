#pragma once
#include "IMetricCollector.h"

struct SystemSnapshot;

class NetCollector : public IMetricCollector
{
public:
    void update(SystemSnapshot &snapshot) override;
    std::string getName() const override;
    unsigned long long getRxKBs() const;
    unsigned long long getTxKBs() const;

private:
    unsigned long long prevRx_ = 0;
    unsigned long long prevTx_ = 0;
    unsigned long long rxSpeedKBs_ = 0;
    unsigned long long txSpeedKBs_ = 0;
};