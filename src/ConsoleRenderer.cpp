#include "ConsoleRenderer.h"
#include "SystemSnapshot.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>

namespace
{
    constexpr int barWidth = 20;
    double normalizePercent(double percent)
    {
        if (percent < 0.0 || !std::isfinite(percent))
        {
            percent = 0.0;
        }
        if (percent > 100.0)
        {
            percent = 100.0;
        }
        return percent;
    }

    std::string stringColor(double percent)
    {
        if (percent < 50.0)
        {
            return "\033[32m";
        }
        else if (percent <= 80.0)
        {
            return "\033[33m";
        }
        else
        {
            return "\033[31m";
        }
    }
    std::string makeBar(double percent)
    {
        std::string result = "[";
        int filled = static_cast<int>(percent / 100 * barWidth);
        for (int i = 1; i <= barWidth; ++i)
        {
            if (i <= filled)
            {
                result += "█";
                continue;
            }
            result += "░";
        }
        result += "]";
        return result;
    }
}

ConsoleRenderer::ConsoleRenderer()
{
    std::cout << "\033[2J";
}

void ConsoleRenderer::renderDashboard(const SystemSnapshot &snapshot)
{
    const double cpuPercent = normalizePercent(snapshot.cpuPercent);
    const double memPercent = normalizePercent(snapshot.memPercent);
    std::cout << std::fixed;
    std::cout << "\033[H";
    std::cout << "CPU: " << makeBar(cpuPercent) << " " << std::setprecision(1) << stringColor(cpuPercent) << cpuPercent << "\033[0m%\033[K\n"
              << "RAM: " << makeBar(memPercent) << " " << snapshot.usedMB << "/" << snapshot.totalMB << " MB ("
              << stringColor(memPercent) << memPercent << "\033[0m" << "%)\033[K\n"
              << "NET: " << "RX : " << snapshot.rxKBs << " KB/s " << " TX : " << snapshot.txKBs << " KB/s" << "\033[K" << std::endl;
}
