#pragma once
#include "model/Order.h"
#include <string>

class OrderView {
public:
    struct OrderInput { std::string sampleId; std::string customerName; int quantity; };
    OrderInput getOrderInput();
    bool confirmOrder(const OrderInput& in);
    void showOrderPlaced(const std::string& orderId);
    void showError(const std::string& msg);
};
