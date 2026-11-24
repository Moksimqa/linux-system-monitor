#include <gtest/gtest.h>
#include "CliParser.h"
#include "Config.h"
#include <string>
#include <vector>

namespace
{
    bool parse(std::vector<std::string> args, Config &config)
    {
        std::vector<char *> argv;

        for (auto &arg : args)
        {
            argv.push_back(arg.data());
        }

        argv.push_back(nullptr);

        return parseArguments(
            static_cast<int>(args.size()),
            argv.data(),
            config);
    }
}

TEST(CliParserTest, UsesDefaultConfig)
{
    Config config;

    ASSERT_TRUE(parse({"monitor"}, config));

    EXPECT_EQ(config.sortMode, SortMode::Cpu);
    EXPECT_EQ(config.intervalMs, 1000u);
    EXPECT_EQ(config.maxProcesses, 15u);
    EXPECT_TRUE(config.colorEnabled);
    EXPECT_TRUE(config.outputPath.empty());
    EXPECT_FALSE(config.helpRequested);
}

TEST(CliParserTest, ParsesMultipleArguments)
{
    Config config;

    ASSERT_TRUE(parse({
        "monitor",
        "--sort", "mem",
        "--interval", "500",
        "--limit", "3",
        "--no-color",
        "--output", "metrics.csv"
    }, config));

    EXPECT_EQ(config.sortMode, SortMode::Rss);
    EXPECT_EQ(config.intervalMs, 500u);
    EXPECT_EQ(config.maxProcesses, 3u);
    EXPECT_FALSE(config.colorEnabled);
    EXPECT_EQ(config.outputPath, "metrics.csv");
}

TEST(CliParserTest, ParsesSortModes)
{
    Config config;

    ASSERT_TRUE(parse({"monitor", "--sort", "cpu"}, config));
    EXPECT_EQ(config.sortMode, SortMode::Cpu);

    ASSERT_TRUE(parse({"monitor", "--sort", "mem"}, config));
    EXPECT_EQ(config.sortMode, SortMode::Rss);

    ASSERT_TRUE(parse({"monitor", "--sort", "pid"}, config));
    EXPECT_EQ(config.sortMode, SortMode::Pid);
}

TEST(CliParserTest, ParsesHelp)
{
    Config config;

    ASSERT_TRUE(parse({"monitor", "--help"}, config));
    EXPECT_TRUE(config.helpRequested);
}

TEST(CliParserTest, RejectsUnknownArgument)
{
    Config config;

    EXPECT_FALSE(parse({"monitor", "--unknown"}, config));
}

TEST(CliParserTest, RejectsInvalidSortMode)
{
    Config config;

    EXPECT_FALSE(parse({"monitor", "--sort", "abc"}, config));
}

TEST(CliParserTest, RejectsMissingValues)
{
    Config config;

    EXPECT_FALSE(parse({"monitor", "--sort"}, config));
    EXPECT_FALSE(parse({"monitor", "--interval"}, config));
    EXPECT_FALSE(parse({"monitor", "--limit"}, config));
    EXPECT_FALSE(parse({"monitor", "--output"}, config));
}

TEST(CliParserTest, RejectsInvalidInterval)
{
    Config config;

    EXPECT_FALSE(parse({"monitor", "--interval", "abc"}, config));
    EXPECT_FALSE(parse({"monitor", "--interval", "500abc"}, config));
    EXPECT_FALSE(parse({"monitor", "--interval", "0"}, config));
    EXPECT_FALSE(parse({"monitor", "--interval", "-500"}, config));
    EXPECT_FALSE(parse({"monitor", "--interval", ""}, config));
    EXPECT_FALSE(parse({
        "monitor", "--interval", "999999999999999999999999"
    }, config));
}

TEST(CliParserTest, RejectsInvalidLimit)
{
    Config config;

    EXPECT_FALSE(parse({"monitor", "--limit", "abc"}, config));
    EXPECT_FALSE(parse({"monitor", "--limit", "12abc"}, config));
    EXPECT_FALSE(parse({"monitor", "--limit", "0"}, config));
    EXPECT_FALSE(parse({"monitor", "--limit", "-5"}, config));
    EXPECT_FALSE(parse({"monitor", "--limit", ""}, config));
    EXPECT_FALSE(parse({
        "monitor", "--limit", "999999999999999999999999"
    }, config));
}

TEST(CliParserTest, RejectsInvalidOutputPath)
{
    Config config;

    EXPECT_FALSE(parse({"monitor", "--output", ""}, config));
    EXPECT_FALSE(parse({"monitor", "--output", "--no-color"}, config));
}

TEST(CliParserTest, PreservesConfigOnFailure)
{
    Config config;

    config.intervalMs = 2000;
    config.maxProcesses = 10;
    config.sortMode = SortMode::Pid;
    config.colorEnabled = false;
    config.outputPath = "old.csv";

    EXPECT_FALSE(parse({
        "monitor",
        "--interval", "500",
        "--limit", "20",
        "--sort", "mem",
        "--output", "new.csv",
        "--unknown"
    }, config));

    EXPECT_EQ(config.intervalMs, 2000u);
    EXPECT_EQ(config.maxProcesses, 10u);
    EXPECT_EQ(config.sortMode, SortMode::Pid);
    EXPECT_FALSE(config.colorEnabled);
    EXPECT_EQ(config.outputPath, "old.csv");
    EXPECT_FALSE(config.helpRequested);
}