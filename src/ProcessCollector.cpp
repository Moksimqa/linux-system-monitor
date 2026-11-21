#include "ProcessCollector.h"
#include "ProcessInfo.h"
#include "ProcessParser.h"
#include "CpuCollector.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <cctype>
#include <fstream>
#include <system_error>
#include <utility>
#include <algorithm>

bool compareByCpu(const ProcessInfo &a, const ProcessInfo &b)
{
    return a.cpuPercent > b.cpuPercent;
}

bool compareByPid(const ProcessInfo &a, const ProcessInfo &b)
{
    return a.pid < b.pid;
}
bool compareByRss(const ProcessInfo &a, const ProcessInfo &b)
{
    return a.rssKB > b.rssKB;
}

std::vector<ProcessInfo> ProcessCollector::collect(SortMode mode)
{
    std::vector<ProcessInfo> local;

    std::ifstream file("/proc/stat");

    if (!file.is_open())
    {
        return local;
    }
    std::string line;
    if (!std::getline(file, line))
    {
        return local;
    }

    unsigned long long total = 0;
    unsigned long long unusedIdle = 0;

    if (!parseCpuStatLine(line, total, unusedIdle))
    {
        return local;
    }
    unsigned long long deltaTotalTicks = 0;
    if (hasPrev_ && total > prevTotalTicks_)
    {
        deltaTotalTicks = total - prevTotalTicks_;
    }
    std::unordered_map<int, ProcessCpuHistory> currentHistory;
    ProcessParser parser;
    std::error_code ec;
    std::filesystem::directory_iterator it("/proc", ec);
    std::filesystem::directory_iterator end;
    if (ec)
    {
        return local;
    }

    for (; it != end && !ec; it.increment(ec))
    {
        std::string name = it->path().filename().string();
        bool isPid = !name.empty();
        for (char c : name)
        {
            if (!std::isdigit(static_cast<unsigned char>(c)))
            {
                isPid = false;
                break;
            }
        }
        if (!isPid)
        {
            continue;
        }
        ProcessInfo process;
        process.pid = std::stoi(name);

        std::string statPath = "/proc/" + name + "/stat";
        std::ifstream statFile(statPath);
        if (!statFile.is_open())
        {
            continue;
        }
        std::string statLine;
        if (!std::getline(statFile, statLine))
        {
            continue;
        }

        if (!parser.parseStatLine(statLine, process))
        {
            continue;
        }

        unsigned long long ticks = process.utime + process.stime;
        auto prevProcessIt = prevProcesses_.find(process.pid);
        if (prevProcessIt != prevProcesses_.end() && process.starttime == prevProcessIt->second.starttime)
        {
            if (ticks >= prevProcessIt->second.ticks && deltaTotalTicks > 0)
            {
                unsigned long long deltaTicks = ticks - prevProcessIt->second.ticks;
                process.cpuPercent = static_cast<double>(deltaTicks) / deltaTotalTicks * 100;
            }
        }
        currentHistory[process.pid] = {ticks, process.starttime};

        std::string statusPath = "/proc/" + name + "/status";
        std::ifstream statusFile(statusPath);
        if (!statusFile.is_open())
        {
            continue;
        }
        std::string statusLine;

        while (std::getline(statusFile, statusLine))
        {
            if (!parser.parseVmRssLine(statusLine, process.rssKB))
            {
                continue;
            }
            break;
        }

        local.push_back(process);
    }
    if (!ec)
    {
        prevProcesses_ = std::move(currentHistory);
        prevTotalTicks_ = total;
        hasPrev_ = true;
    }
    switch (mode)
    {
    case SortMode::Cpu:
        std::sort(local.begin(), local.end(), compareByCpu);
        break;
    case SortMode::Pid:
        std::sort(local.begin(), local.end(), compareByPid);
        break;
    case SortMode::Rss:
        std::sort(local.begin(), local.end(), compareByRss);
        break;
    }
    return local;
}
