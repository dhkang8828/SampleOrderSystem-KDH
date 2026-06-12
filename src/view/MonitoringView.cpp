#include "view/MonitoringView.h"
#include <iostream>
#include <iomanip>

void MonitoringView::showOrderCounts(const std::map<std::string, int>& counts) {
    std::cout << "\n--- Order Status Summary ---\n";
    for (const auto& kv : counts) {
        std::cout << std::setw(12) << kv.first << ": " << kv.second << "\n";
    }
}

void MonitoringView::showStockStatus(const std::vector<Sample>& samples) {
    std::cout << "\n--- Stock Status ---\n";
    std::cout << std::left << std::setw(8) << "ID"
              << std::setw(16) << "Name"
              << std::setw(8) << "Stock"
              << "Status\n";
    std::cout << std::string(44, '-') << "\n";
    for (const auto& s : samples) {
        std::string status;
        if (s.getStock() == 0)       status = "DEPLETED";
        else if (s.getStock() < 50)  status = "LOW";
        else                          status = "OK";
        std::cout << std::setw(8)  << s.getSampleId()
                  << std::setw(16) << s.getName()
                  << std::setw(8)  << s.getStock()
                  << status << "\n";
    }
}
