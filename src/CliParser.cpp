#include "CliParser.h"
#include "Config.h"
#include <string>
#include <charconv>
#include <system_error>
#include <iostream>

bool parseArguments(int argc, char *argv[], Config &config)
{
    Config localConfig;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--sort")
        {
            if (i + 1 >= argc)
            {
                return false;
            }
            std::string value = argv[++i];

            if (value == "cpu")
            {
                localConfig.sortMode = SortMode::Cpu;
            }
            else if (value == "mem")
            {
                localConfig.sortMode = SortMode::Rss;
            }
            else if (value == "pid")
            {
                localConfig.sortMode = SortMode::Pid;
            }
            else
            {
                return false;
            }
        }
        else if (arg == "--no-color")
        {
            localConfig.colorEnabled = false;
        }
        else if (arg == "--interval")
        {
            if (i + 1 >= argc)
            {
                return false;
            }

            std::string interval = argv[++i];
            unsigned int value = 0;

            const char *begin = interval.data();
            const char *end = begin + interval.size();
            auto result = std::from_chars(begin, end, value);

            if (result.ec != std::errc{} || result.ptr != end || value == 0)
            {
                return false;
            }

            localConfig.intervalMs = value;
        }
        else if (arg == "--limit")
        {
            if (i + 1 >= argc)
            {
                return false;
            }
            std::string limit = argv[++i];
            std::size_t value = 0;

            const char *begin = limit.data();
            const char *end = begin + limit.size();
            auto result = std::from_chars(begin, end, value);

            if (result.ec != std::errc{} || result.ptr != end || value == 0)
            {
                return false;
            }

            localConfig.maxProcesses = value;
        }
        else if (arg == "--output")
        {
            if (i + 1 >= argc)
            {
                return false;
            }
            std::string path = argv[++i];
            if (path.empty() || path.front() == '-')
            {
                return false;
            }
            localConfig.outputPath = path;
        }
        else if (arg == "--help")
        {
            localConfig.helpRequested = true;
        }
        else
        {
            return false;
        }
    }
    config = localConfig;
    return true;
}

void printHelp()
{
    std::cout
        << "Usage: monitor [options]\n\n"
        << "Options:\n"
        << "  --help              Show this help message\n"
        << "  --sort cpu|mem|pid  Sort processes (default: cpu)\n"
        << "  --interval <ms>     Update interval (default: 1000)\n"
        << "  --limit <N>         Process limit (default: 15)\n"
        << "  --no-color          Disable colors\n"
        << "  --output <path>     Save metrics to a CSV file\n";
}