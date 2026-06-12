#include "model/ProductionJob.h"

ProductionJob::ProductionJob(int queueId, const std::string& orderId,
                             int actualProductionQty, double totalProductionTime,
                             const std::string& enqueuedAt)
    : queueId_(queueId), orderId_(orderId),
      actualProductionQty_(actualProductionQty), totalProductionTime_(totalProductionTime),
      enqueuedAt_(enqueuedAt) {}
