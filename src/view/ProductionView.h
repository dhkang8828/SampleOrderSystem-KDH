#pragma once
#include "model/ProductionJob.h"
#include <vector>
#include <string>

class ProductionView {
public:
    void showQueue(const std::vector<ProductionJob>& jobs);
    std::string selectOrderId();
    void showCompleted(const std::string& orderId);
};
