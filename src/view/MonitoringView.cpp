#include "view/MonitoringView.h"
#include <iostream>
#include <iomanip>

void MonitoringView::showOrderCounts(const std::map<std::string, int>& counts) {
    std::cout << "\n--- Order Status Summary ---\n";
    for (const auto& kv : counts) {
        std::cout << std::setw(12) << kv.first << ": " << kv.second << "\n";
    }
}

void MonitoringView::showStockStatus(const std::vector<Sample>& samples,
                                     const std::map<std::string, int>& pendingQty) {
    std::cout << "\n--- Stock Status ---\n";
    std::cout << std::left
              << std::setw(8)  << "ID"
              << std::setw(16) << "Name"
              << std::setw(8)  << "Stock"
              << std::setw(10) << "Pending"
              << "Status\n";
    std::cout << std::string(54, '-') << "\n";

    for (const auto& s : samples) {
        auto it = pendingQty.find(s.getSampleId());
        int pending = (it != pendingQty.end()) ? it->second : 0;

        std::string status;
        if (s.getStock() == 0)
            status = "DEPLETED";
        else if (s.getStock() < pending)
            status = "LACK";
        else
            status = "OK";

        std::cout << std::setw(8)  << s.getSampleId()
                  << std::setw(16) << s.getName()
                  << std::setw(8)  << s.getStock()
                  << std::setw(10) << pending
                  << status << "\n";
    }
}
