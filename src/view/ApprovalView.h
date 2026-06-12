#pragma once
#include "model/Order.h"
#include <vector>
#include <string>

class ApprovalView {
public:
    void showReservedOrders(const std::vector<Order>& orders);
    std::string selectOrderId();
    int getApprovalAction();
    void showResult(const std::string& orderId, const std::string& action);
};
