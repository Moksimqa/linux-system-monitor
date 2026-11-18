#include <gtest/gtest.h>
#include "CpuCollector.h"
#include <string>

TEST(CpuCollectorTests, ParsesValidCpuStatLine)
{
    const std::string line = "cpu 100 20 30 400 5 6 7 8";

    unsigned long long total = 0;
    unsigned long long idle = 0;

    bool result = parseCpuStatLine(line, total, idle);

    EXPECT_TRUE(result);
    EXPECT_EQ(total, 576);
    EXPECT_EQ(idle, 405);
}

TEST(CpuCollectorTests, RejectsIncompleteCpuStatLine)
{
    const std::string line = "cpu 100 20 30";

    unsigned long long total = 111;
    unsigned long long idle = 222;

    bool result = parseCpuStatLine(line, total, idle);

    EXPECT_FALSE(result);
    EXPECT_EQ(total, 111);
    EXPECT_EQ(idle, 222);
}

TEST(CpuCollectorTests, RejectsInvalidCpuStatLine)
{
    const std::string line = "cpu 100 20 30 BAD 5 6 7 8";

    unsigned long long total = 111;
    unsigned long long idle = 222;

    bool result = parseCpuStatLine(line, total, idle);

    EXPECT_FALSE(result);
    EXPECT_EQ(total, 111);
    EXPECT_EQ(idle, 222);
}