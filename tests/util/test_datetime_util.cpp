#include <gtest/gtest.h>
#include "util/DateTimeUtil.h"
#include <regex>

TEST(DateTimeUtilTest, NowReturnsISO8601Format) {
    std::string now = DateTimeUtil::now();
    // e.g. 2026-06-12T14:30:00
    std::regex iso8601(R"(\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2})");
    EXPECT_TRUE(std::regex_match(now, iso8601));
}

TEST(DateTimeUtilTest, TodayReturnsYYYYMMDD) {
    std::string today = DateTimeUtil::today();
    // e.g. 20260612
    std::regex yyyymmdd(R"(\d{8})");
    EXPECT_TRUE(std::regex_match(today, yyyymmdd));
}

TEST(DateTimeUtilTest, TodayLengthIsEight) {
    EXPECT_EQ(8u, DateTimeUtil::today().size());
}
