#include <gtest/gtest.h>
#include "model/Sample.h"

TEST(SampleTest, HasEnoughStock_WhenSufficient) {
    Sample s("S-001", "TestSample", 10.0, 0.92, 100);
    EXPECT_TRUE(s.hasEnoughStock(50));
    EXPECT_TRUE(s.hasEnoughStock(100));
}

TEST(SampleTest, HasEnoughStock_WhenInsufficient) {
    Sample s("S-001", "TestSample", 10.0, 0.92, 30);
    EXPECT_FALSE(s.hasEnoughStock(50));
}

TEST(SampleTest, GetShortfall_WhenInsufficient) {
    Sample s("S-001", "TestSample", 10.0, 0.92, 30);
    EXPECT_EQ(20, s.getShortfall(50));
}

TEST(SampleTest, GetShortfall_WhenSufficient) {
    Sample s("S-001", "TestSample", 10.0, 0.92, 100);
    EXPECT_EQ(0, s.getShortfall(50));
}

TEST(SampleTest, GetStockStatus_Sufficient) {
    Sample s("S-001", "TestSample", 10.0, 0.92, 100);
    EXPECT_EQ(StockStatus::SUFFICIENT, s.getStockStatus(50));
}

TEST(SampleTest, GetStockStatus_Shortage) {
    Sample s("S-001", "TestSample", 10.0, 0.92, 30);
    EXPECT_EQ(StockStatus::SHORTAGE, s.getStockStatus(50));
}

TEST(SampleTest, GetStockStatus_Depleted) {
    Sample s("S-001", "TestSample", 10.0, 0.92, 0);
    EXPECT_EQ(StockStatus::DEPLETED, s.getStockStatus(10));
}

TEST(SampleTest, Accessors) {
    Sample s("S-001", "AlphaX", 5.5, 0.85, 200);
    EXPECT_EQ("S-001", s.getSampleId());
    EXPECT_EQ("AlphaX", s.getName());
    EXPECT_DOUBLE_EQ(5.5, s.getAvgProductionTime());
    EXPECT_DOUBLE_EQ(0.85, s.getYieldRate());
    EXPECT_EQ(200, s.getStock());
}
