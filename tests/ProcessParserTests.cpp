#include <gtest/gtest.h>
#include "ProcessParser.h"
#include "ProcessInfo.h"
#include <string>

TEST(ProcessParserTest, ParsesValidStatLine)
{
    ProcessParser parser;
    ProcessInfo process;
    const std::string testString = "123 (my process) S "
                                   "1 1 1 0 -1 0 0 0 0 0 "
                                   "100 50 "
                                   "0 0 20 0 1 0 "
                                   "777";
    bool result = parser.parseStatLine(testString, process);

    EXPECT_TRUE(result);
    EXPECT_EQ(process.name, "my process");
    EXPECT_EQ(process.state, 'S');
    EXPECT_EQ(process.utime, 100);
    EXPECT_EQ(process.stime, 50);
    EXPECT_EQ(process.starttime, 777);
}

TEST(ProcessParserTest, RejectsMissingCloseBracket)
{
    ProcessParser parser;
    ProcessInfo process;
    const std::string testString = "123 (my process S 1 2 3";
    bool result = parser.parseStatLine(testString, process);

    EXPECT_FALSE(result);
}

TEST(ProcessParserTest, RejectsReversedBrackets)
{
    ProcessParser parser;
    ProcessInfo process;
    const std::string testString = "123 )broken( S 1 2 3";
    bool result = parser.parseStatLine(testString, process);

    EXPECT_FALSE(result);
}

TEST(ProcessParserTest, ParsesValidVmRssLine)
{
    ProcessParser parser;
    ProcessInfo process;
    unsigned long long rssKB = 0;
    const std::string testString = "VmRSS:      12345 kB";

    bool result = parser.parseVmRssLine(testString, rssKB);

    EXPECT_TRUE(result);
    EXPECT_EQ(rssKB, 12345);
}

TEST(ProcessParserTest, RejectsInvalidVmRssValue)
{
    ProcessParser parser;
    ProcessInfo process;
    unsigned long long rssKB = 777;
    const std::string testString = "VmRSS:      garbage kB";

    bool result = parser.parseVmRssLine(testString, rssKB);

    EXPECT_FALSE(result);
    EXPECT_EQ(rssKB, 777);
}
