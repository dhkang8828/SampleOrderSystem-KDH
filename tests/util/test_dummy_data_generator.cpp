#include <gtest/gtest.h>
#include "util/DummyDataGenerator.h"
#include "util/DBManager.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"

class DummyDataGeneratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
        sqlite3* db = DBManager::getInstance().getDB();
        sampleRepo = std::make_unique<SampleRepository>(db);
        orderRepo  = std::make_unique<OrderRepository>(db);
    }
    void TearDown() override {
        sampleRepo.reset();
        orderRepo.reset();
        DBManager::getInstance().close();
    }

    std::unique_ptr<SampleRepository> sampleRepo;
    std::unique_ptr<OrderRepository>  orderRepo;
};

TEST_F(DummyDataGeneratorTest, Generate_InsertsSampleCount) {
    DummyDataGenerator::generate(*sampleRepo, *orderRepo, 42);
    EXPECT_EQ(DummyDataGenerator::SAMPLE_COUNT,
              static_cast<int>(sampleRepo->findAll().size()));
}

TEST_F(DummyDataGeneratorTest, Generate_InsertsOrderCount) {
    DummyDataGenerator::generate(*sampleRepo, *orderRepo, 42);
    int expected = DummyDataGenerator::SAMPLE_COUNT * DummyDataGenerator::ORDERS_PER_SAMPLE;
    EXPECT_EQ(expected, static_cast<int>(orderRepo->findAll().size()));
}

TEST_F(DummyDataGeneratorTest, Generate_AllOrdersAreReserved) {
    DummyDataGenerator::generate(*sampleRepo, *orderRepo, 42);
    auto orders = orderRepo->findAll();
    for (const auto& o : orders) {
        EXPECT_EQ(OrderStatus::RESERVED, o.getStatus());
    }
}

TEST_F(DummyDataGeneratorTest, Generate_OrdersSampleIdsExistInSampleRepo) {
    DummyDataGenerator::generate(*sampleRepo, *orderRepo, 42);
    auto orders = orderRepo->findAll();
    for (const auto& o : orders) {
        EXPECT_TRUE(sampleRepo->existsById(o.getSampleId()))
            << "Order references unknown sampleId: " << o.getSampleId();
    }
}

TEST_F(DummyDataGeneratorTest, Generate_SamplesHaveValidValues) {
    DummyDataGenerator::generate(*sampleRepo, *orderRepo, 42);
    auto samples = sampleRepo->findAll();
    for (const auto& s : samples) {
        EXPECT_GT(s.getAvgProductionTime(), 0.0);
        EXPECT_GT(s.getYieldRate(), 0.0);
        EXPECT_LE(s.getYieldRate(), 1.0);
        EXPECT_GE(s.getStock(), 0);
    }
}

TEST_F(DummyDataGeneratorTest, Generate_TwiceCumulatesData) {
    DummyDataGenerator::generate(*sampleRepo, *orderRepo, 1);
    DummyDataGenerator::generate(*sampleRepo, *orderRepo, 2);
    EXPECT_EQ(DummyDataGenerator::SAMPLE_COUNT * 2,
              static_cast<int>(sampleRepo->findAll().size()));
    EXPECT_EQ(DummyDataGenerator::SAMPLE_COUNT * DummyDataGenerator::ORDERS_PER_SAMPLE * 2,
              static_cast<int>(orderRepo->findAll().size()));
}
