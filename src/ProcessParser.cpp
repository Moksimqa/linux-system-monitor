#include "ProcessParser.h"
#include "ProcessInfo.h"
#include <sstream>

bool ProcessParser::parseStatLine(const std::string &statLine, ProcessInfo &process)
{
    std::size_t openBracket = statLine.find('(');
    std::size_t closeBracket = statLine.rfind(')');

    if (openBracket == std::string::npos ||
        closeBracket == std::string::npos ||
        closeBracket <= openBracket ||
        closeBracket + 2 >= statLine.size())
    {
        return false;
    }
    std::string processName = statLine.substr(openBracket + 1, closeBracket - openBracket - 1);

    std::istringstream ss(statLine.substr(closeBracket + 2));
    char state;

    if (!(ss >> state))
    {
        return false;
    }

    long long dummy = 0;
    unsigned long long localUTime = 0;
    unsigned long long localSTime = 0;
    unsigned long long localstartTime = 0;

    for (int i = 0; i < 10; ++i)
    {
        if (!(ss >> dummy))
        {
            return false;
        }
    }
    if (!(ss >> localUTime))
    {
        return false;
    }
    if (!(ss >> localSTime))
    {
        return false;
    }

    for (int i = 0; i < 6; ++i)
    {
        if (!(ss >> dummy))
        {
            return false;
        }
    }
    if (!(ss >> localstartTime))
    {
        return false;
    }

    process.utime = localUTime;
    process.stime = localSTime;
    process.starttime = localstartTime;
    process.name = processName;
    process.state = state;
    return true;
}

bool ProcessParser::parseVmRssLine(const std::string &statusLine, unsigned long long &rssKB)
{
    std::istringstream ss(statusLine);
    std::string fieldName;
    unsigned long long localRss = 0;
    ss >> fieldName;
    if (fieldName != "VmRSS:")
    {
        return false;
    }
    if (!(ss >> localRss))
    {
        return false;
    }
    rssKB = localRss;
    return true;
}