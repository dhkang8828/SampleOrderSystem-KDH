#include <gtest/gtest.h>
#include "repository/OrderRepository.h"
#include "util/DBManager.h"

class OrderRepoTest : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
        repo = std::make_unique<OrderRepository>(DBManager::getInstance().getDB());
    }
    void TearDown() override {
        repo.reset();
        DBManager::getInstance().close();
    }
    std::unique_ptr<OrderRepository> repo;

    Order makeOrder(const std::string& id = "ORD-20260612-0001") {
        return Order(id, "S-001", "CustomerA", 50, "2026-06-12T10:00:00");
    }
};

TEST_F(OrderRepoTest, SaveAndFindById) {
    repo->save(makeOrder());
    auto result = repo->findById("ORD-20260612-0001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ("CustomerA", result->getCustomerName());
    EXPECT_EQ(OrderStatus::RESERVED, result->getStatus());
}

TEST_F(OrderRepoTest, FindById_NotFound) {
    auto result = repo->findById("ORD-99999999-0001");
    EXPECT_FALSE(result.has_value());
}

TEST_F(OrderRepoTest, FindByStatus_Reserved) {
    repo->save(makeOrder("ORD-20260612-0001"));
    repo->save(makeOrder("ORD-20260612-0002"));
    auto reserved = repo->findByStatus(OrderStatus::RESERVED);
    EXPECT_EQ(2u, reserved.size());
}

TEST_F(OrderRepoTest, UpdateStatus) {
    repo->save(makeOrder());
    repo->updateStatus("ORD-20260612-0001", OrderStatus::CONFIRMED);
    auto result = repo->findById("ORD-20260612-0001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(OrderStatus::CONFIRMED, result->getStatus());
}

TEST_F(OrderRepoTest, CountByStatus) {
    repo->save(makeOrder("ORD-20260612-0001"));
    repo->save(makeOrder("ORD-20260612-0002"));
    repo->updateStatus("ORD-20260612-0001", OrderStatus::CONFIRMED);
    EXPECT_EQ(1, repo->countByStatus(OrderStatus::CONFIRMED));
    EXPECT_EQ(1, repo->countByStatus(OrderStatus::RESERVED));
}

TEST_F(OrderRepoTest, GenerateNextOrderSequence_FirstOfDay) {
    int seq = repo->generateNextOrderSequence("20260612");
    EXPECT_EQ(1, seq);
}

TEST_F(OrderRepoTest, GenerateNextOrderSequence_Increment) {
    repo->save(makeOrder("ORD-20260612-0001"));
    int seq = repo->generateNextOrderSequence("20260612");
    EXPECT_EQ(2, seq);
}
