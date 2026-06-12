#include "controller/ReleaseController.h"
#include <stdexcept>

ReleaseController::ReleaseController(OrderRepository* orderRepo,
                                     SampleRepository* sampleRepo,
                                     ProductionQueueRepository* prodQueueRepo)
    : orderRepo_(orderRepo), sampleRepo_(sampleRepo), prodQueueRepo_(prodQueueRepo) {}

void ReleaseController::releaseOrder(const std::string& orderId) {
    auto orderOpt = orderRepo_->findById(orderId);
    if (!orderOpt) throw std::runtime_error("Order not found: " + orderId);
    if (!orderOpt->isConfirmed()) throw std::runtime_error("Order is not in CONFIRMED state");

    auto sampleOpt = sampleRepo_->findById(orderOpt->getSampleId());
    if (!sampleOpt) throw std::runtime_error("Sample not found");

    int newStock = sampleOpt->getStock() - orderOpt->getQuantity();
    sampleRepo_->updateStock(sampleOpt->getSampleId(), newStock);
    orderRepo_->updateStatus(orderId, OrderStatus::RELEASED);
}

void ReleaseController::completeProduction(const std::string& orderId, int producedQty) {
    auto orderOpt = orderRepo_->findById(orderId);
    if (!orderOpt) throw std::runtime_error("Order not found: " + orderId);

    auto sampleOpt = sampleRepo_->findById(orderOpt->getSampleId());
    if (!sampleOpt) throw std::runtime_error("Sample not found");

    int newStock = sampleOpt->getStock() + producedQty;
    sampleRepo_->updateStock(sampleOpt->getSampleId(), newStock);
    orderRepo_->updateStatus(orderId, OrderStatus::CONFIRMED);
}
