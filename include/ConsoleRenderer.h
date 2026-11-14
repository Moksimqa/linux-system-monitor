#pragma once
#include <vector>
#include "ProcessInfo.h"
struct SystemSnapshot;

class ConsoleRenderer
{
public:
    ConsoleRenderer();
    void renderDashboard(const SystemSnapshot &snapshot, const std::vector<ProcessInfo> &processes);

private:
    void renderSystemMetrics(const SystemSnapshot &snapshot);
    void renderProcessTable(const std::vector<ProcessInfo> &processes);
};
