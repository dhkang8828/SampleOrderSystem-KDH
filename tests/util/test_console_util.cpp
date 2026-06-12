#include <gtest/gtest.h>
#include "util/ConsoleUtil.h"
#include <sstream>

// Tests for ConsoleUtil::readValidInt using in-memory streams.

TEST(ConsoleUtilTest, ReadValidInt_ValidInRange_ReturnsValue) {
    std::istringstream in("3\n");
    std::ostringstream out;
    int result = ConsoleUtil::readValidInt(in, out, 0, 5, "retry: ");
    EXPECT_EQ(3, result);
    EXPECT_TRUE(out.str().empty());
}

TEST(ConsoleUtilTest, ReadValidInt_BoundaryMin_Accepted) {
    std::istringstream in("0\n");
    std::ostringstream out;
    EXPECT_EQ(0, ConsoleUtil::readValidInt(in, out, 0, 6, "retry: "));
}

TEST(ConsoleUtilTest, ReadValidInt_BoundaryMax_Accepted) {
    std::istringstream in("6\n");
    std::ostringstream out;
    EXPECT_EQ(6, ConsoleUtil::readValidInt(in, out, 0, 6, "retry: "));
}

TEST(ConsoleUtilTest, ReadValidInt_TooLarge_PrintsErrorAndRetries) {
    std::istringstream in("9\n2\n");
    std::ostringstream out;
    int result = ConsoleUtil::readValidInt(in, out, 0, 5, "retry: ");
    EXPECT_EQ(2, result);
    EXPECT_EQ("retry: ", out.str());
}

TEST(ConsoleUtilTest, ReadValidInt_Negative_PrintsErrorAndRetries) {
    std::istringstream in("-1\n1\n");
    std::ostringstream out;
    int result = ConsoleUtil::readValidInt(in, out, 0, 5, "retry: ");
    EXPECT_EQ(1, result);
    EXPECT_EQ("retry: ", out.str());
}

TEST(ConsoleUtilTest, ReadValidInt_MultipleInvalid_RetriesUntilValid) {
    std::istringstream in("7\n-3\n4\n");
    std::ostringstream out;
    int result = ConsoleUtil::readValidInt(in, out, 0, 5, "retry: ");
    EXPECT_EQ(4, result);
    EXPECT_EQ("retry: retry: ", out.str());
}

TEST(ConsoleUtilTest, ReadValidInt_NonNumericInput_PrintsErrorAndRetries) {
    std::istringstream in("abc\n2\n");
    std::ostringstream out;
    int result = ConsoleUtil::readValidInt(in, out, 0, 5, "retry: ");
    EXPECT_EQ(2, result);
    EXPECT_EQ("retry: ", out.str());
}

TEST(ConsoleUtilTest, ReadValidInt_MixedInvalidThenValid) {
    std::istringstream in("xyz\n10\n0\n");
    std::ostringstream out;
    int result = ConsoleUtil::readValidInt(in, out, 0, 3, "retry: ");
    EXPECT_EQ(0, result);
    EXPECT_EQ("retry: retry: ", out.str());
}
