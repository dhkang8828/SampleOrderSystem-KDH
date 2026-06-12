#include <gtest/gtest.h>
#include "model/ProductionJob.h"

TEST(ProductionJobTest, Accessors) {
    ProductionJob job(1, "ORD-20260612-0001", 206, 2060.0, "2026-06-12T10:00:00");
    EXPECT_EQ(1, job.getQueueId());
    EXPECT_EQ("ORD-20260612-0001", job.getOrderId());
    EXPECT_EQ(206, job.getActualProductionQty());
    EXPECT_DOUBLE_EQ(2060.0, job.getTotalProductionTime());
    EXPECT_EQ("2026-06-12T10:00:00", job.getEnqueuedAt());
}
