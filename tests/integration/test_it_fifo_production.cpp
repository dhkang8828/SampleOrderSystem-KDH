#include "IntegrationTestBase.h"

// IT-05: FIFO production queue ordering
// Orders must be processed in the order they were enqueued.

class IntegrationTest_FifoProduction : public IntegrationTestBase {};

TEST_F(IntegrationTest_FifoProduction, FIFO_FirstApprovedIsFirstInQueue) {
    std::string sid  = registerSample("DeltaW", 12.0, 0.90, 0);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpD", 50);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpE", 80);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    auto firstJob = prodQueueRepo->peek();
    ASSERT_TRUE(firstJob.has_value());
    EXPECT_EQ(oid1, firstJob->getOrderId());
}

TEST_F(IntegrationTest_FifoProduction, FIFO_SecondOrderRemainsProducingAfterFirst) {
    std::string sid  = registerSample("DeltaW", 12.0, 0.90, 0);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpD", 50);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpE", 80);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    completeFirstJob();

    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid1)->getStatus());
    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid2)->getStatus());
    EXPECT_FALSE(prodQueueRepo->isEmpty());
}

TEST_F(IntegrationTest_FifoProduction, FIFO_ThreeOrders_ProcessedInOrder) {
    std::string sid  = registerSample("DeltaW", 12.0, 0.90, 0);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpD", 30);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpE", 40);
    std::string oid3 = orderCtrl->placeOrder(sid, "CorpF", 20);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);
    orderCtrl->approveOrder(oid3);

    std::string done1 = completeFirstJob();
    EXPECT_EQ(oid1, done1);
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid1)->getStatus());
    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid2)->getStatus());
    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid3)->getStatus());

    std::string done2 = completeFirstJob();
    EXPECT_EQ(oid2, done2);
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid2)->getStatus());
    EXPECT_EQ(OrderStatus::PRODUCING, orderRepo->findById(oid3)->getStatus());

    std::string done3 = completeFirstJob();
    EXPECT_EQ(oid3, done3);
    EXPECT_EQ(OrderStatus::CONFIRMED, orderRepo->findById(oid3)->getStatus());
    EXPECT_TRUE(prodQueueRepo->isEmpty());
}

TEST_F(IntegrationTest_FifoProduction, FIFO_FullFlow_BothOrdersReleasedInOrder) {
    std::string sid  = registerSample("DeltaW", 12.0, 0.90, 0);
    std::string oid1 = orderCtrl->placeOrder(sid, "CorpD", 50);
    std::string oid2 = orderCtrl->placeOrder(sid, "CorpE", 80);
    orderCtrl->approveOrder(oid1);
    orderCtrl->approveOrder(oid2);

    completeFirstJob();
    releaseCtrl->releaseOrder(oid1);

    completeFirstJob();
    releaseCtrl->releaseOrder(oid2);

    EXPECT_EQ(OrderStatus::RELEASED, orderRepo->findById(oid1)->getStatus());
    EXPECT_EQ(OrderStatus::RELEASED, orderRepo->findById(oid2)->getStatus());
    EXPECT_TRUE(prodQueueRepo->isEmpty());
}
