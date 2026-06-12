#include "view/ProductionView.h"
#include <iostream>
#include <iomanip>
#include <limits>

void ProductionView::showQueue(const std::vector<ProductionJob>& jobs) {
    if (jobs.empty()) {
        std::cout << "Production queue is empty.\n";
        return;
    }
    std::cout << std::left
              << std::setw(6)  << "QueueID"
              << std::setw(24) << "OrderID"
              << std::setw(8)  << "Qty"
              << "TotalTime(min)\n";
    std::cout << std::string(52, '-') << "\n";
    for (const auto& j : jobs) {
        std::cout << std::setw(6)  << j.getQueueId()
                  << std::setw(24) << j.getOrderId()
                  << std::setw(8)  << j.getActualProductionQty()
                  << j.getTotalProductionTime() << "\n";
    }
}

std::string ProductionView::selectOrderId() {
    std::string id;
    std::cout << "Enter Order ID to complete production: ";
    std::getline(std::cin, id);
    return id;
}

void ProductionView::showCompleted(const std::string& orderId) {
    std::cout << "Production completed: " << orderId << " -> CONFIRMED\n";
}
