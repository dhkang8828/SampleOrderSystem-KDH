#pragma once
#include "repository/OrderRepository.h"
#include "repository/SampleRepository.h"
#include "repository/ProductionQueueRepository.h"
#include <string>

class ReleaseController {
public:
    ReleaseController(OrderRepository* orderRepo,
                      SampleRepository* sampleRepo,
                      ProductionQueueRepository* prodQueueRepo);

    void releaseOrder(const std::string& orderId);
    void completeProduction(const std::string& orderId, int producedQty);

private:
    OrderRepository* orderRepo_;
    SampleRepository* sampleRepo_;
    ProductionQueueRepository* prodQueueRepo_;
};
