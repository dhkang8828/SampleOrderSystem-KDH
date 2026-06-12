#include <gtest/gtest.h>
#include "repository/ProductionQueueRepository.h"
#include "util/DBManager.h"

class ProdQueueRepoTest : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
        repo = std::make_unique<ProductionQueueRepository>(DBManager::getInstance().getDB());
    }
    void TearDown() override {
        repo.reset();
        DBManager::getInstance().close();
    }
    std::unique_ptr<ProductionQueueRepository> repo;

    ProductionJob makeJob(const std::string& orderId) {
        return ProductionJob(0, orderId, 206, 2060.0, "2026-06-12T10:00:00");
    }
};

TEST_F(ProdQueueRepoTest, IsEmpty_Initially) {
    EXPECT_TRUE(repo->isEmpty());
}

TEST_F(ProdQueueRepoTest, EnqueueAndPeek) {
    repo->enqueue(makeJob("ORD-20260612-0001"));
    auto top = repo->peek();
    ASSERT_TRUE(top.has_value());
    EXPECT_EQ("ORD-20260612-0001", top->getOrderId());
}

TEST_F(ProdQueueRepoTest, DequeueRemovesItem) {
    repo->enqueue(makeJob("ORD-20260612-0001"));
    repo->dequeue();
    EXPECT_TRUE(repo->isEmpty());
}

TEST_F(ProdQueueRepoTest, FIFOOrder) {
    repo->enqueue(makeJob("ORD-20260612-0001"));
    repo->enqueue(makeJob("ORD-20260612-0002"));
    auto first = repo->dequeue();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ("ORD-20260612-0001", first->getOrderId());
    auto second = repo->dequeue();
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ("ORD-20260612-0002", second->getOrderId());
}

TEST_F(ProdQueueRepoTest, DequeueOnEmpty_ReturnsNullopt) {
    auto result = repo->dequeue();
    EXPECT_FALSE(result.has_value());
}

TEST_F(ProdQueueRepoTest, FindAll_OrderedByQueueId) {
    repo->enqueue(makeJob("ORD-20260612-0001"));
    repo->enqueue(makeJob("ORD-20260612-0002"));
    auto all = repo->findAll();
    ASSERT_EQ(2u, all.size());
    EXPECT_EQ("ORD-20260612-0001", all[0].getOrderId());
    EXPECT_EQ("ORD-20260612-0002", all[1].getOrderId());
}
