#include <gtest/gtest.h>
#include "controller/ProductionController.h"
#include <cmath>

TEST(ProductionControllerTest, CalcActualProductionQty_BasicCase) {
    // shortfall=170, yield=0.92 -> ceil(170 / (0.92 * 0.9)) = ceil(205.21) = 206
    int qty = ProductionController::calcActualProductionQty(170, 0.92);
    EXPECT_EQ(206, qty);
}

TEST(ProductionControllerTest, CalcActualProductionQty_NoShortfall) {
    int qty = ProductionController::calcActualProductionQty(0, 0.92);
    EXPECT_EQ(0, qty);
}

TEST(ProductionControllerTest, CalcActualProductionQty_ExactResult) {
    // shortfall=90, yield=1.0 -> ceil(90 / (1.0 * 0.9)) = ceil(100) = 100
    int qty = ProductionController::calcActualProductionQty(90, 1.0);
    EXPECT_EQ(100, qty);
}

TEST(ProductionControllerTest, CalcTotalProductionTime) {
    // avgTime=10.0, qty=206 -> 2060.0
    double time = ProductionController::calcTotalProductionTime(10.0, 206);
    EXPECT_DOUBLE_EQ(2060.0, time);
}
