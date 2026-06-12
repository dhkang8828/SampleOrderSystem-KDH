#pragma once
#include <string>

class ProductionJob {
public:
    ProductionJob(int queueId, const std::string& orderId,
                  int actualProductionQty, double totalProductionTime,
                  const std::string& enqueuedAt);

    int getQueueId() const { return queueId_; }
    const std::string& getOrderId() const { return orderId_; }
    int getActualProductionQty() const { return actualProductionQty_; }
    double getTotalProductionTime() const { return totalProductionTime_; }
    const std::string& getEnqueuedAt() const { return enqueuedAt_; }

private:
    int queueId_;
    std::string orderId_;
    int actualProductionQty_;
    double totalProductionTime_;
    std::string enqueuedAt_;
};
