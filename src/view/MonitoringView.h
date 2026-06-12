#pragma once
#include "model/Sample.h"
#include <vector>
#include <map>
#include <string>

class MonitoringView {
public:
    void showOrderCounts(const std::map<std::string, int>& counts);
    // pendingQty: sampleId -> total qty of RESERVED+PRODUCING orders
    void showStockStatus(const std::vector<Sample>& samples,
                         const std::map<std::string, int>& pendingQty);
};
