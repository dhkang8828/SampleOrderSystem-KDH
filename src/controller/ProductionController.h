#pragma once

class ProductionController {
public:
    ProductionController() = delete;
    static int calcActualProductionQty(int shortfall, double yieldRate);
    static double calcTotalProductionTime(double avgProductionTime, int actualQty);
};
