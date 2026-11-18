#pragma once
#include "IMetricCollector.h"
#include <istream>

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

bool parseMemInfo(std::istream &input, unsigned long long &total, unsigned long long &available);