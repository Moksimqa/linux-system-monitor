#include "CpuCollector.h"
#include "MemCollector.h"
#include "SystemSnapshot.h"
#include "NetCollector.h"
#include "ConsoleRenderer.h"
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>
#include <csignal>

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

std::atomic<bool> g_running{true};
void signalHandler(int signum)
{
    (void)signum;
    g_running = false;
}

int main()
{
    SystemSnapshot snapshot;
    ConsoleRenderer renderer;
    std::mutex mtx;
    std::signal(SIGINT, signalHandler);
    std::thread worker(dataCollectorWorker, std::ref(snapshot), std::ref(mtx), std::ref(g_running));
    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        SystemSnapshot local;
        {
            std::lock_guard<std::mutex> lock(mtx);
            local = snapshot;
        }
        renderer.renderDashboard(local);
    }
    worker.join();
}
