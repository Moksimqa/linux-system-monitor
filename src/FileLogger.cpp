#include "FileLogger.h"
#include "SystemSnapshot.h"
#include <chrono>

FileLogger::FileLogger(const std::string &path)
    : file_(path)
{
    if (file_.is_open())
    {
        file_ << "timestamp_ms,cpu_percent,mem_percent,rx_kbs,tx_kbs\n";
    }
}

bool FileLogger::isOpen() const
{
    return file_.is_open();
}

void FileLogger::log(const SystemSnapshot &snapshot)
{
    if (!file_.is_open())
    {
        return;
    }

    auto now = std::chrono::system_clock::now();

    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();

    file_ << timestamp << ','
          << snapshot.cpuPercent << ','
          << snapshot.memPercent << ','
          << snapshot.rxKBs << ','
          << snapshot.txKBs << '\n';
}