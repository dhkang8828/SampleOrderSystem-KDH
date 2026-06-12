#include "model/Order.h"

Order::Order(const std::string& orderId, const std::string& sampleId,
             const std::string& customerName, int quantity,
             const std::string& createdAt)
    : orderId_(orderId), sampleId_(sampleId), customerName_(customerName),
      quantity_(quantity), status_(OrderStatus::RESERVED), createdAt_(createdAt) {}
