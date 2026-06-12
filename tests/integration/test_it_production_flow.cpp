#include "IntegrationTestBase.h"

// IT-02: Production flow (insufficient stock)
// RESERVED -> PRODUCING -> CONFIRMED -> RELEASED
// Formula: actualQty = ceil(shortfall / (yieldRate * 0.9))

class IntegrationTest_ProductionFlow : public IntegrationTestBase {};

TEST_F(IntegrationTest_ProductionFlow, Approve_InsufficientStock_BecomesProducing) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);

    orderCtrl->approveOrder(oid);

    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid)->getStatus());
    EXPECT_FALSE(prodQueueRepo->isEmpty());
    EXPECT_EQ(30, sampleRepo->findById(sid)->getStock());
}

TEST_F(IntegrationTest_ProductionFlow, ProductionQty_Formula_IsCorrect) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);
    orderCtrl->approveOrder(oid);

    auto job = prodQueueRepo->peek();
    ASSERT_TRUE(job.has_value());
    // shortfall=170, ceil(170 / (0.92*0.9)) = ceil(205.3) = 206
    EXPECT_EQ(206, job->getActualProductionQty());
}

TEST_F(IntegrationTest_ProductionFlow, CompleteProduction_IncreasesStockAndConfirms) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);
    orderCtrl->approveOrder(oid);

    completeFirstJob();

    EXPECT_EQ(236, sampleRepo->findById(sid)->getStock());  // 30 + 206
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());
    EXPECT_TRUE(prodQueueRepo->isEmpty());
}

TEST_F(IntegrationTest_ProductionFlow, FullProductionFlow_FinalStockIsCorrect) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);
    orderCtrl->approveOrder(oid);
    completeFirstJob();
    releaseCtrl->releaseOrder(oid);

    EXPECT_EQ(36, sampleRepo->findById(sid)->getStock());   // 236 - 200
    EXPECT_EQ(OrderStatus::RELEASED, orderRepo->findById(oid)->getStatus());
}

TEST_F(IntegrationTest_ProductionFlow, FullFlow_StatusTransitions) {
    std::string sid = registerSample("GammaZ", 8.0, 0.92, 30);
    std::string oid = orderCtrl->placeOrder(sid, "CorpB", 200);

    EXPECT_EQ(OrderStatus::RESERVED,  orderRepo->findById(oid)->getStatus());

    orderCtrl->approveOrder(oid);
    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid)->getStatus());

    completeFirstJob();
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());

    releaseCtrl->releaseOrder(oid);
    EXPECT_EQ(OrderStatus::RELEASED,  orderRepo->findById(oid)->getStatus());
}
