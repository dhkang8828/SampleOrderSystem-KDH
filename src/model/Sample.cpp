#include "model/Sample.h"
#include <algorithm>

Sample::Sample(const std::string& sampleId, const std::string& name,
               double avgProductionTime, double yieldRate, int stock)
    : sampleId_(sampleId), name_(name),
      avgProductionTime_(avgProductionTime), yieldRate_(yieldRate), stock_(stock) {}

bool Sample::hasEnoughStock(int quantity) const {
    return stock_ >= quantity;
}

int Sample::getShortfall(int quantity) const {
    return std::max(0, quantity - stock_);
}

StockStatus Sample::getStockStatus(int quantity) const {
    if (stock_ == 0) return StockStatus::DEPLETED;
    if (stock_ < quantity) return StockStatus::SHORTAGE;
    return StockStatus::SUFFICIENT;
}
