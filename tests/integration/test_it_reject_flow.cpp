#include "IntegrationTestBase.h"

// IT-03: Order rejection flow
// Rejecting an order must leave stock and production queue untouched.

class IntegrationTest_RejectFlow : public IntegrationTestBase {};

TEST_F(IntegrationTest_RejectFlow, Reject_DoesNotChangeStock) {
    std::string sid = registerSample("BetaY", 15.0, 0.90, 100);
    std::string oid = orderCtrl->placeOrder(sid, "CorpC", 50);

    orderCtrl->rejectOrder(oid);

    EXPECT_EQ(100, sampleRepo->findById(sid)->getStock());
    EXPECT_EQ(OrderStatus::REJECTED, orderRepo->findById(oid)->getStatus());
}

TEST_F(IntegrationTest_RejectFlow, Reject_DoesNotEnqueueProductionJob) {
    std::string sid = registerSample("BetaY", 15.0, 0.90, 10);
    std::string oid = orderCtrl->placeOrder(sid, "CorpC", 100);

    orderCtrl->rejectOrder(oid);

    EXPECT_TRUE(prodQueueRepo->isEmpty());
}

TEST_F(IntegrationTest_RejectFlow, Reject_NotInConfirmedList) {
    std::string sid = registerSample("BetaY", 15.0, 0.90, 100);
    std::string oid = orderCtrl->placeOrder(sid, "CorpC", 50);

    orderCtrl->rejectOrder(oid);

    EXPECT_TRUE(orderRepo->findByStatus(OrderStatus::CONFIRMED).empty());
}

TEST_F(IntegrationTest_RejectFlow, Reject_CannotBeReleased) {
    std::string sid = registerSample("BetaY", 15.0, 0.90, 100);
    std::string oid = orderCtrl->placeOrder(sid, "CorpC", 50);

    orderCtrl->rejectOrder(oid);

    EXPECT_THROW(releaseCtrl->releaseOrder(oid), std::runtime_error);
}

TEST_F(IntegrationTest_RejectFlow, RejectOne_ApproveAnother_IndependentlyProcessed) {
    std::string sid  = registerSample("BetaY", 15.0, 0.90, 100);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpC", 50);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpD", 30);

    orderCtrl->rejectOrder(oid1);
    orderCtrl->approveOrder(oid2);

    EXPECT_EQ(OrderStatus::REJECTED,  orderRepo->findById(oid1)->getStatus());
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid2)->getStatus());
    EXPECT_EQ(100, sampleRepo->findById(sid)->getStock());
}
