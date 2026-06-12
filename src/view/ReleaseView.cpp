#include "view/ReleaseView.h"
#include <iostream>
#include <iomanip>
#include <limits>

void ReleaseView::showConfirmedOrders(const std::vector<Order>& orders) {
    if (orders.empty()) {
        std::cout << "No orders ready for release.\n";
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

std::string ReleaseView::selectOrderId() {
    std::string id;
    std::cout << "Enter Order ID to release: ";
    std::getline(std::cin, id);
    return id;
}

void ReleaseView::showReleased(const std::string& orderId) {
    std::cout << "Released: " << orderId << "\n";
}
