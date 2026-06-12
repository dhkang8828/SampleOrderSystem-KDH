#include <gtest/gtest.h>
#include "controller/OrderController.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionQueueRepository.h"
#include "util/DBManager.h"

class OrderControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
        sqlite3* db = DBManager::getInstance().getDB();
        sampleRepo   = std::make_unique<SampleRepository>(db);
        orderRepo    = std::make_unique<OrderRepository>(db);
        prodQueueRepo = std::make_unique<ProductionQueueRepository>(db);
        controller   = std::make_unique<OrderController>(
            sampleRepo.get(), orderRepo.get(), prodQueueRepo.get());

        // Insert a sample with stock=100
        sampleRepo->save(Sample("S-001", "AlphaX", 10.0, 0.92, 100));
    }
    void TearDown() override {
        controller.reset();
        sampleRepo.reset();
        orderRepo.reset();
        prodQueueRepo.reset();
        DBManager::getInstance().close();
    }

    std::unique_ptr<SampleRepository> sampleRepo;
    std::unique_ptr<OrderRepository> orderRepo;
    std::unique_ptr<ProductionQueueRepository> prodQueueRepo;
    std::unique_ptr<OrderController> controller;
};

TEST_F(OrderControllerTest, PlaceOrder_CreatesReservedOrder) {
    std::string orderId = controller->placeOrder("S-001", "CustomerA", 50);
    EXPECT_FALSE(orderId.empty());
    auto order = orderRepo->findById(orderId);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(OrderStatus::RESERVED, order->getStatus());
}

TEST_F(OrderControllerTest, PlaceOrder_InvalidSample_Throws) {
    EXPECT_THROW(controller->placeOrder("S-999", "CustomerA", 50), std::runtime_error);
}

TEST_F(OrderControllerTest, ApproveOrder_StockSufficient_BecomesConfirmed) {
    std::string orderId = controller->placeOrder("S-001", "CustomerA", 50);
    controller->approveOrder(orderId);
    auto order = orderRepo->findById(orderId);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(OrderStatus::CONFIRMED, order->getStatus());
    // stock is deducted at release time, not at approval
    auto sample = sampleRepo->findById("S-001");
    EXPECT_EQ(100, sample->getStock());
}

TEST_F(OrderControllerTest, ApproveOrder_StockInsufficient_BecomesProducing) {
    // stock=100, order=150 -> shortage -> PRODUCING
    std::string orderId = controller->placeOrder("S-001", "CustomerA", 150);
    controller->approveOrder(orderId);
    auto order = orderRepo->findById(orderId);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(OrderStatus::PRODUCING, order->getStatus());
    // production job should be enqueued
    EXPECT_FALSE(prodQueueRepo->isEmpty());
}

TEST_F(OrderControllerTest, RejectOrder_BecomesRejected) {
    std::string orderId = controller->placeOrder("S-001", "CustomerA", 50);
    controller->rejectOrder(orderId);
    auto order = orderRepo->findById(orderId);
    ASSERT_TRUE(order.has_value());
    EXPECT_EQ(OrderStatus::REJECTED, order->getStatus());
}
