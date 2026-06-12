#pragma once
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionQueueRepository.h"
#include <string>

class OrderController {
public:
    OrderController(SampleRepository* sampleRepo,
                    OrderRepository* orderRepo,
                    ProductionQueueRepository* prodQueueRepo);

    std::string placeOrder(const std::string& sampleId,
                           const std::string& customerName,
                           int quantity);

    void approveOrder(const std::string& orderId);
    void rejectOrder(const std::string& orderId);

private:
    SampleRepository* sampleRepo_;
    OrderRepository* orderRepo_;
    ProductionQueueRepository* prodQueueRepo_;
};
