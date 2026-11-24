#include "CpuCollector.h"
#include "MemCollector.h"
#include "SystemSnapshot.h"
#include "NetCollector.h"
#include "ConsoleRenderer.h"
#include "ProcessCollector.h"
#include "Config.h"
#include "CliParser.h"
#include "FileLogger.h"
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>
#include <csignal>
#include <iostream>
#include <chrono>

void dataCollectorWorker(SystemSnapshot &snapshot, std::mutex &mtx, std::atomic<bool> &running, unsigned int interval)
{
    std::vector<std::unique_ptr<IMetricCollector>> collectors;
    collectors.push_back(std::make_unique<CpuCollector>());
    collectors.push_back(std::make_unique<MemCollector>());
    collectors.push_back(std::make_unique<NetCollector>());
    SystemSnapshot dummy;
    for (auto &c : collectors)
    {
        c->update(dummy);
    }

    while (running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));

        SystemSnapshot localSnapshot;
        for (auto &c : collectors)
        {
            c->update(localSnapshot);
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            snapshot = localSnapshot;
        }
    }
}

std::atomic<bool> g_running{true};
void signalHandler(int signum)
{
    (void)signum;
    g_running = false;
}

int main(int argc, char *argv[])
{
    Config config;
    if (!parseArguments(argc, argv, config))
    {
        std::cerr << "Error: invalid command-line arguments.\n";
        return 1;
    }
    if (config.helpRequested)
    {
        printHelp();
        return 0;
    }

    std::unique_ptr<FileLogger> logger;
    if (!config.outputPath.empty())
    {
        logger = std::make_unique<FileLogger>(config.outputPath);
        if (!(logger->isOpen()))
        {
            std::cerr << "Error: Failed to open file: " << config.outputPath << '\n';
            return 1;
        }
    }

    SystemSnapshot snapshot;
    ProcessCollector processCollector;
    ConsoleRenderer renderer(config.maxProcesses, config.colorEnabled);
    std::mutex mtx;
    std::signal(SIGINT, signalHandler);
    std::thread worker(dataCollectorWorker, std::ref(snapshot), std::ref(mtx), std::ref(g_running), config.intervalMs);
    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(config.intervalMs));
        SystemSnapshot local;
        {
            std::lock_guard<std::mutex> lock(mtx);
            local = snapshot;
        }
        auto processes = processCollector.collect(config.sortMode);
        if(logger){
            logger->log(local);
        }
        renderer.renderDashboard(local, processes);
    }
    worker.join();
}
