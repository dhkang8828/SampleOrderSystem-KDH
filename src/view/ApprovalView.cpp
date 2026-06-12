#include "view/ApprovalView.h"
#include <iostream>
#include <iomanip>
#include <limits>

void ApprovalView::showReservedOrders(const std::vector<Order>& orders) {
    if (orders.empty()) {
        std::cout << "No pending orders.\n";
        return;
    }
    std::cout << std::left
              << std::setw(24) << "OrderID"
              << std::setw(8)  << "SampleID"
              << std::setw(14) << "Customer"
              << std::setw(6)  << "Qty" << "\n";
    std::cout << std::string(52, '-') << "\n";
    for (const auto& o : orders) {
        std::cout << std::setw(24) << o.getOrderId()
                  << std::setw(8)  << o.getSampleId()
                  << std::setw(14) << o.getCustomerName()
                  << std::setw(6)  << o.getQuantity() << "\n";
    }
}

std::string ApprovalView::selectOrderId() {
    std::string id;
    std::cout << "Enter Order ID: ";
    std::getline(std::cin, id);
    return id;
}

int ApprovalView::getApprovalAction() {
    std::cout << "[1] Approve  [2] Reject  [0] Cancel\nSelect: ";
    int choice = -1;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

void ApprovalView::showResult(const std::string& orderId, const std::string& action) {
    std::cout << "Order " << orderId << " -> " << action << "\n";
}
