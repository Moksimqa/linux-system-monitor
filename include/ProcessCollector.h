#pragma once
#include "ProcessInfo.h"
#include "SortMode.h"
#include <vector>
#include <unordered_map>

struct ProcessCpuHistory
{
    unsigned long long ticks = 0;
    unsigned long long starttime = 0;
};


class ProcessCollector
{

public:
    std::vector<ProcessInfo> collect(SortMode mode = SortMode::Cpu);

private:
    std::unordered_map<int, ProcessCpuHistory> prevProcesses_;
    unsigned long long prevTotalTicks_ = 0;
    bool hasPrev_ = false;
};