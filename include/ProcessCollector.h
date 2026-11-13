#pragma once
#include <vector>
#include "ProcessInfo.h"

class ProcessCollector
{
public:
    std::vector<ProcessInfo> collect();
};