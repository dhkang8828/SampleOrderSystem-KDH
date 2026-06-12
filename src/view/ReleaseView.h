#pragma once
#include "model/Order.h"
#include <vector>
#include <string>

class ReleaseView {
public:
    void showConfirmedOrders(const std::vector<Order>& orders);
    std::string selectOrderId();
    void showReleased(const std::string& orderId);
};
