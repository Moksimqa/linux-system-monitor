#include <gtest/gtest.h>

#include "NetCollector.h"

#include <sstream>
#include <string>
#include <unordered_map>

TEST(NetCollectorTests, ParsesMultipleInterfaces)
{
    const std::string inputText =
        "Inter-|   Receive                                                |  Transmit\n"
        " face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed\n"
        "    lo: 1000 1 0 0 0 0 0 0 2000 1 0 0 0 0 0 0\n"
        "  eth0: 5000 10 0 0 0 0 0 0 6000 20 0 0 0 0 0 0\n"
        " wlan0: 7000 30 0 0 0 0 0 0 8000 40 0 0 0 0 0 0\n";

    std::istringstream input(inputText);

    std::unordered_map<std::string, NetInterfaceCounters> interfaces;

    bool result = parseNetDev(input, interfaces);

    EXPECT_TRUE(result);

    EXPECT_EQ(interfaces.size(), 2);

    EXPECT_EQ(interfaces.at("eth0").rxBytes, 5000);
    EXPECT_EQ(interfaces.at("eth0").txBytes, 6000);

    EXPECT_EQ(interfaces.at("wlan0").rxBytes, 7000);
    EXPECT_EQ(interfaces.at("wlan0").txBytes, 8000);

    EXPECT_EQ(interfaces.count("lo"), 0);
}

TEST(NetCollectorTests, AcceptsOnlyLoopbackInterface)
{
    const std::string inputText =
        "Inter-|   Receive                                                |  Transmit\n"
        " face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed\n"
        "    lo: 1000 1 0 0 0 0 0 0 2000 1 0 0 0 0 0 0\n";

    std::istringstream input(inputText);

    std::unordered_map<std::string, NetInterfaceCounters> interfaces;

    bool result = parseNetDev(input, interfaces);

    EXPECT_TRUE(result);
    EXPECT_TRUE(interfaces.empty());
}

TEST(NetCollectorTests, RejectsMalformedInterfaceLine)
{
    const std::string inputText =
        "Inter-|   Receive                                                |  Transmit\n"
        " face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed\n"
        "  eth0: 5000 10 0 BAD 0 0 0 0 6000 20 0 0 0 0 0 0\n";

    std::istringstream input(inputText);

    std::unordered_map<std::string, NetInterfaceCounters> interfaces{
        {"old0", {111, 222}}};

    bool result = parseNetDev(input, interfaces);

    EXPECT_FALSE(result);

    EXPECT_EQ(interfaces.size(), 1);
    EXPECT_EQ(interfaces.at("old0").rxBytes, 111);
    EXPECT_EQ(interfaces.at("old0").txBytes, 222);
}