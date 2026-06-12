#include "IntegrationTestBase.h"

// IT-04: Multiple orders on the same sample
// Approvals do not touch stock; each release deducts only its own quantity.

class IntegrationTest_MultiOrderFlow : public IntegrationTestBase {};

TEST_F(IntegrationTest_MultiOrderFlow, MultipleApprovals_StockUnchanged) {
    std::string sid  = registerSample("AlphaX", 10.0, 0.92, 300);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpA", 80);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpB", 120);

    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    EXPECT_EQ(300, sampleRepo->findById(sid)->getStock());
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid1)->getStatus());
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid2)->getStatus());
}

TEST_F(IntegrationTest_MultiOrderFlow, SequentialRelease_CumulativeStockDeduction) {
    std::string sid  = registerSample("AlphaX", 10.0, 0.92, 300);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpA", 80);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpB", 120);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    releaseCtrl->releaseOrder(oid1);
    EXPECT_EQ(220, sampleRepo->findById(sid)->getStock());  // 300-80

    releaseCtrl->releaseOrder(oid2);
    EXPECT_EQ(100, sampleRepo->findById(sid)->getStock());  // 220-120
}

TEST_F(IntegrationTest_MultiOrderFlow, ReleaseOrder_DeductsOnlyItsOwnQuantity) {
    std::string sid  = registerSample("AlphaX", 10.0, 0.92, 300);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpA", 80);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpB", 120);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    releaseCtrl->releaseOrder(oid2);  // release oid2 first
    EXPECT_EQ(180, sampleRepo->findById(sid)->getStock());  // 300-120

    releaseCtrl->releaseOrder(oid1);
    EXPECT_EQ(100, sampleRepo->findById(sid)->getStock());  // 180-80
}

TEST_F(IntegrationTest_MultiOrderFlow, ThreeOrders_AllApprovedAsConfirmed) {
    std::string sid  = registerSample("AlphaX", 10.0, 0.92, 200);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpA", 80);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpB", 100);
    std::string oid3 = orderCtrl->placeOrder(sid, "CorpC", 90);

    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);
    orderCtrl->approveOrder(oid3);  // stock=200 >= 90, so CONFIRMED

    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid1)->getStatus());
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid2)->getStatus());
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid3)->getStatus());
    EXPECT_EQ(200, sampleRepo->findById(sid)->getStock());  // no deduction at approve
}
