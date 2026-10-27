#pragma once
#include "IMetricCollector.h"

struct SystemSnapshot;

class MemCollector : public IMetricCollector
{
public:
    void update(SystemSnapshot &snapshot) override;
    std::string getName() const override;

    unsigned long long getUsedMB() const;
    unsigned long long getTotalMB() const;
    double getMemPercent() const;

private:
    double memPercent_ = 0.0;
    unsigned long long memTotalKB_ = 0;
    unsigned long long memAvailableKB_ = 0;
};