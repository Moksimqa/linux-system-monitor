#pragma once
#include <string>

struct SystemSnapshot;

struct IMetricCollector
{

    virtual ~IMetricCollector() = default;

    virtual void update(SystemSnapshot &snapshot) = 0;

    virtual std::string getName() const = 0;
};