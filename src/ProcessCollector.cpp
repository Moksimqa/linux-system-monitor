#include "ProcessCollector.h"
#include "ProcessInfo.h"
#include <vector>
#include <string>
#include <filesystem>
#include <cctype>
#include <fstream>
#include <sstream>

std::vector<ProcessInfo> ProcessCollector::collect()
{
    std::vector<ProcessInfo> local;
    for (const auto &entry : std::filesystem::directory_iterator("/proc"))
    {
        std::string name = entry.path().filename().string();
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
        size_t openBracket = statLine.find('(');
        size_t closeBracket = statLine.rfind(')');

        if (openBracket == std::string::npos ||
            closeBracket == std::string::npos ||
            closeBracket + 2 >= statLine.size())
        {
            continue;
        }
        std::string processName = statLine.substr(openBracket + 1, closeBracket - openBracket - 1);
        char state = statLine[closeBracket + 2];
        process.name = processName;
        process.state = state;

        std::string statusPath = "/proc/" + name + "/status";
        std::ifstream statusFile(statusPath);
        if (!statusFile.is_open())
        {
            continue;
        }
        std::string statusLine;

        while (std::getline(statusFile, statusLine))
        {
            std::istringstream ss(statusLine);
            std::string fieldName;
            ss >> fieldName;
            if (fieldName != "VmRSS:")
            {
                continue;
            }
            ss >> process.rssKB;
            break;
        }

        local.push_back(process);
    }
    return local;
}
