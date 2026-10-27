#include "CpuCollector.h"
#include "MemCollector.h"
#include "SystemSnapshot.h"
#include "NetCollector.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>
#include <csignal>

/*
void dataCollectorWorker(SystemSnapshot &snapshot, std::mutex &mtx, std::atomic<bool> &running)
{
    CpuCollector cpu;
    MemCollector mem;
    NetCollector net;
    cpu.update();
    net.update();
    while (running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        cpu.update();
        mem.update();
        net.update();
        std::lock_guard<std::mutex> lock(mtx);
        snapshot.cpuPercent = cpu.getCpuPercent();
        snapshot.memPercent = mem.getMemPercent();
        snapshot.usedMB = mem.getUsedMB();
        snapshot.totalMB = mem.getTotalMB();
        snapshot.rxKBs = net.getRxKBs();
        snapshot.txKBs = net.getTxKBs();
    }
}
    */

void dataCollectorWorker(SystemSnapshot &snapshot, std::mutex &mtx, std::atomic<bool> &running)
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
        std::this_thread::sleep_for(std::chrono::seconds(1));

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

std::string stringColor(double percent)
{
    if (percent < 50.0)
    {
        return "\033[32m";
    }
    else if (percent >= 50.0 && percent <= 80.0)
    {
        return "\033[33m";
    }
    else if (percent > 80)
    {
        return "\033[31m";
    }
    return "\033[0m";
}
void renderDashboard(const SystemSnapshot &snapshot)
{
    std::cout << "\033[H";
    std::cout << "CPU: " << stringColor(snapshot.cpuPercent) << snapshot.cpuPercent << "\033[0m" << "% | RAM: "
              << snapshot.usedMB << "/" << snapshot.totalMB << " MB ("
              << stringColor(snapshot.memPercent) << snapshot.memPercent << "\033[0m" << "%)" << " | NET: " << "Receive (KB/s) : " << snapshot.rxKBs << " Transmit (KB/s) : " << snapshot.txKBs << std::endl;
}

std::atomic<bool> g_running{true};
void signalHandler(int signum)
{
    (void)signum;
    g_running = false;
}

int main()
{
    SystemSnapshot snapshot;
    std::mutex mtx;
    std::signal(SIGINT, signalHandler);
    std::thread worker(dataCollectorWorker, std::ref(snapshot), std::ref(mtx), std::ref(g_running));
    std::cout << "\033[2J";
    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        SystemSnapshot local;
        {
            std::lock_guard<std::mutex> lock(mtx);
            local = snapshot;
        }
        renderDashboard(local);
    }
    worker.join();
}