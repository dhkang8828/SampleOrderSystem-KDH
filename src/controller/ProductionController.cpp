#include "controller/ProductionController.h"
#include <cmath>

int ProductionController::calcActualProductionQty(int shortfall, double yieldRate) {
    if (shortfall <= 0) return 0;
    return static_cast<int>(std::ceil(shortfall / (yieldRate * 0.9)));
}

double ProductionController::calcTotalProductionTime(double avgProductionTime, int actualQty) {
    return avgProductionTime * actualQty;
}
