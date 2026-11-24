#pragma once
#include <vector>
#include "ProcessInfo.h"
struct SystemSnapshot;

class ConsoleRenderer
{
public:
    ConsoleRenderer(std::size_t maxProcesses, bool colorEnabled);
    void renderDashboard(const SystemSnapshot &snapshot, const std::vector<ProcessInfo> &processes);

private:
    std::size_t maxProcesses_ = 15;
    bool colorEnabled_ = true;
    void renderSystemMetrics(const SystemSnapshot &snapshot);
    void renderProcessTable(const std::vector<ProcessInfo> &processes);
};
