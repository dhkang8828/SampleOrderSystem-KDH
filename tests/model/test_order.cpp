#include <gtest/gtest.h>
#include "model/Order.h"

TEST(OrderTest, DefaultStatusIsReserved) {
    Order o("ORD-20260612-0001", "S-001", "CustomerA", 50, "2026-06-12T10:00:00");
    EXPECT_EQ(OrderStatus::RESERVED, o.getStatus());
}

TEST(OrderTest, IsReserved) {
    Order o("ORD-20260612-0001", "S-001", "CustomerA", 50, "2026-06-12T10:00:00");
    EXPECT_TRUE(o.isReserved());
    EXPECT_FALSE(o.isConfirmed());
    EXPECT_FALSE(o.isProducing());
    EXPECT_FALSE(o.isReleased());
    EXPECT_FALSE(o.isRejected());
}

TEST(OrderTest, SetStatusToConfirmed) {
    Order o("ORD-20260612-0001", "S-001", "CustomerA", 50, "2026-06-12T10:00:00");
    o.setStatus(OrderStatus::CONFIRMED);
    EXPECT_TRUE(o.isConfirmed());
}

TEST(OrderTest, SetStatusToProducing) {
    Order o("ORD-20260612-0001", "S-001", "CustomerA", 50, "2026-06-12T10:00:00");
    o.setStatus(OrderStatus::PRODUCING);
    EXPECT_TRUE(o.isProducing());
}

TEST(OrderTest, SetStatusToReleased) {
    Order o("ORD-20260612-0001", "S-001", "CustomerA", 50, "2026-06-12T10:00:00");
    o.setStatus(OrderStatus::RELEASED);
    EXPECT_TRUE(o.isReleased());
}

TEST(OrderTest, Accessors) {
    Order o("ORD-20260612-0001", "S-001", "CustomerA", 50, "2026-06-12T10:00:00");
    EXPECT_EQ("ORD-20260612-0001", o.getOrderId());
    EXPECT_EQ("S-001", o.getSampleId());
    EXPECT_EQ("CustomerA", o.getCustomerName());
    EXPECT_EQ(50, o.getQuantity());
    EXPECT_EQ("2026-06-12T10:00:00", o.getCreatedAt());
}
