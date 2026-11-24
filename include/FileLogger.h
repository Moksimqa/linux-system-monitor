#pragma once
#include <string>
#include <fstream>

struct SystemSnapshot;

class FileLogger
{
public:
    explicit FileLogger(const std::string &path);

    bool isOpen() const;
    void log(const SystemSnapshot &snapshot);

private:
    std::ofstream file_;
};
