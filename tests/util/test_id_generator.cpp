#include <gtest/gtest.h>
#include "util/IdGenerator.h"
#include <regex>

TEST(IdGeneratorTest, GenerateOrderId_Format) {
    // e.g. ORD-20260612-0001
    std::string id = IdGenerator::generateOrderId("20260612", 1);
    EXPECT_EQ("ORD-20260612-0001", id);
}

TEST(IdGeneratorTest, GenerateOrderId_PadsSequence) {
    std::string id = IdGenerator::generateOrderId("20260612", 42);
    EXPECT_EQ("ORD-20260612-0042", id);
}

TEST(IdGeneratorTest, GenerateSampleId_Format) {
    // e.g. S-001
    std::string id = IdGenerator::generateSampleId(1);
    EXPECT_EQ("S-001", id);
}

TEST(IdGeneratorTest, GenerateSampleId_Pads) {
    std::string id = IdGenerator::generateSampleId(99);
    EXPECT_EQ("S-099", id);
}
