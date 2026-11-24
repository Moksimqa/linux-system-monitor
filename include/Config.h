#pragma once
#include "SortMode.h"
#include <string>
#include <cstddef>

struct Config
{
    std::size_t maxProcesses = 15;
    unsigned int intervalMs = 1000;
    SortMode sortMode = SortMode::Cpu;
    bool colorEnabled = true;
    std::string outputPath = "";
    bool helpRequested = false;
};
