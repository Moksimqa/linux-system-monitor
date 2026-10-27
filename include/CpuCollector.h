#pragma once
#include "IMetricCollector.h"

struct SystemSnapshot;

class CpuCollector : public IMetricCollector
{

public:
    void update(SystemSnapshot &snapshot) override;
    std::string getName() const override;
    double getCpuPercent() const;

private:
    double cpuPercent_ = 0.0;
    unsigned long long prevTotalTicks_ = 0;
    unsigned long long prevIdleTicks_ = 0;
};