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
    char state = statLine[closeBracket + 2];
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