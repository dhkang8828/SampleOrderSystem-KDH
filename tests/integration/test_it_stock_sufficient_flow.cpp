#include "IntegrationTestBase.h"

// IT-01: Stock-sufficient full flow
// RESERVED -> CONFIRMED -> RELEASED
// Stock must be deducted exactly once, at release time (not at approval).

class IntegrationTest_StockSufficientFlow : public IntegrationTestBase {};

TEST_F(IntegrationTest_StockSufficientFlow, Approve_DoesNotDeductStock) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 200);
    std::string oid = orderCtrl->placeOrder(sid, "CorpA", 100);

    orderCtrl->approveOrder(oid);

    EXPECT_EQ(200, sampleRepo->findById(sid)->getStock());
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());
}

TEST_F(IntegrationTest_StockSufficientFlow, Release_DeductsStockExactlyOnce) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 200);
    std::string oid = orderCtrl->placeOrder(sid, "CorpA", 100);

    orderCtrl->approveOrder(oid);
    releaseCtrl->releaseOrder(oid);

    EXPECT_EQ(100, sampleRepo->findById(sid)->getStock());
    EXPECT_EQ(OrderStatus::RELEASED, orderRepo->findById(oid)->getStatus());
}

TEST_F(IntegrationTest_StockSufficientFlow, FullFlow_StatusTransitions) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 200);
    std::string oid = orderCtrl->placeOrder(sid, "CorpA", 100);

    EXPECT_EQ(OrderStatus::RESERVED,  orderRepo->findById(oid)->getStatus());

    orderCtrl->approveOrder(oid);
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());

    releaseCtrl->releaseOrder(oid);
    EXPECT_EQ(OrderStatus::RELEASED,  orderRepo->findById(oid)->getStatus());
}

TEST_F(IntegrationTest_StockSufficientFlow, ExactStock_ApprovesAsConfirmed) {
    std::string sid = registerSample("AlphaX", 10.0, 0.92, 50);
    std::string oid = orderCtrl->placeOrder(sid, "CorpA", 50);

    orderCtrl->approveOrder(oid);

    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid)->getStatus());
    EXPECT_TRUE(prodQueueRepo->isEmpty());
}
