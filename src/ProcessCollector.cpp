#include "ProcessCollector.h"
#include "ProcessInfo.h"
#include "ProcessParser.h"
#include <vector>
#include <string>
#include <filesystem>
#include <cctype>
#include <fstream>
#include <system_error>

std::vector<ProcessInfo> ProcessCollector::collect()
{
    std::vector<ProcessInfo> local;
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
    return local;
}
