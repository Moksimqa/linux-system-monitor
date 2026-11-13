#pragma once

struct SystemSnapshot;

class ConsoleRenderer
{
public:
    ConsoleRenderer();
    void renderDashboard(const SystemSnapshot &snapshot);
};
