#include <gtest/gtest.h>
#include "MemCollector.h"
#include <sstream>
#include <string>

TEST(MemCollectorTests, ParsesValidMemInfo)
{

    const std::string testString =
        "MemTotal: 8192 kB\n"
        "MemAvailable: 4096 kB\n";

    std::istringstream input(testString);

    unsigned long long total = 0;
    unsigned long long available = 0;

    bool result = parseMemInfo(input, total, available);

    EXPECT_TRUE(result);
    EXPECT_EQ(total, 8192);
    EXPECT_EQ(available, 4096);
}

TEST(MemCollectorTests, AcceptsZeroMemAvailable)
{

    const std::string testString =
        "MemTotal: 8192 kB\n"
        "MemAvailable: 0 kB\n";

    std::istringstream input(testString);

    unsigned long long total = 0;
    unsigned long long available = 0;

    bool result = parseMemInfo(input, total, available);

    EXPECT_TRUE(result);
    EXPECT_EQ(total, 8192);
    EXPECT_EQ(available, 0);
}

TEST(MemCollectorTests, RejectsAvailableGreaterThanTotal)
{

    const std::string testString =
        "MemTotal: 8192 kB\n"
        "MemAvailable: 9000 kB\n";

    std::istringstream input(testString);

    unsigned long long total = 111;
    unsigned long long available = 222;

    bool result = parseMemInfo(input, total, available);

    EXPECT_FALSE(result);
    EXPECT_EQ(total, 111);
    EXPECT_EQ(available, 222);
}
