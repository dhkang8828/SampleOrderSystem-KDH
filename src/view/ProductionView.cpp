#include "view/ProductionView.h"
#include "util/DateTimeUtil.h"
#include <iostream>
#include <iomanip>

void ProductionView::showQueue(const std::vector<ProductionJob>& jobs) {
    std::cout << "=== Production Line Status ===\n";
    if (jobs.empty()) {
        std::cout << "Production queue is empty.\n";
        return;
    }

    const auto& current = jobs[0];
    double elapsed   = DateTimeUtil::elapsedMinutesSince(current.getEnqueuedAt());
    double remaining = current.getTotalProductionTime() - elapsed;

    std::cout << "\n[Currently Producing]\n";
    std::cout << "  Order   : " << current.getOrderId() << "\n";
    std::cout << "  Qty     : " << current.getActualProductionQty() << " ea\n";
    std::cout << "  Total   : " << std::fixed << std::setprecision(1)
              << current.getTotalProductionTime() << " min\n";
    std::cout << "  Elapsed : " << std::fixed << std::setprecision(1) << elapsed << " min\n";
    if (remaining > 0.0) {
        std::cout << "  Remain  : " << std::fixed << std::setprecision(1) << remaining << " min\n";
    } else {
        std::cout << "  Status  : [Completing at next screen refresh]\n";
    }

    if (jobs.size() > 1) {
        std::cout << "\n[Waiting Queue]\n";
        std::cout << std::left
                  << std::setw(4)  << "#"
                  << std::setw(26) << "OrderID"
                  << std::setw(8)  << "Qty"
                  << "TotalTime(min)\n";
        std::cout << std::string(50, '-') << "\n";
        for (size_t i = 1; i < jobs.size(); ++i) {
            std::cout << std::setw(4)  << i
                      << std::setw(26) << jobs[i].getOrderId()
                      << std::setw(8)  << jobs[i].getActualProductionQty()
                      << std::fixed << std::setprecision(1)
                      << jobs[i].getTotalProductionTime() << "\n";
        }
    }
}
