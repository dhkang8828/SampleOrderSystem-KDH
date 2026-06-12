#include "controller/OrderController.h"
#include "controller/ProductionController.h"
#include "util/DateTimeUtil.h"
#include "util/IdGenerator.h"
#include <stdexcept>

OrderController::OrderController(SampleRepository* sampleRepo,
                                 OrderRepository* orderRepo,
                                 ProductionQueueRepository* prodQueueRepo)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), prodQueueRepo_(prodQueueRepo) {}

std::string OrderController::placeOrder(const std::string& sampleId,
                                        const std::string& customerName,
                                        int quantity) {
    if (!sampleRepo_->existsById(sampleId)) {
        throw std::runtime_error("Sample not found: " + sampleId);
    }

    std::string dateStr = DateTimeUtil::today();
    int seq = orderRepo_->generateNextOrderSequence(dateStr);
    std::string orderId = IdGenerator::generateOrderId(dateStr, seq);
    std::string createdAt = DateTimeUtil::now();

    Order order(orderId, sampleId, customerName, quantity, createdAt);
    orderRepo_->save(order);
    return orderId;
}

void OrderController::approveOrder(const std::string& orderId) {
    auto orderOpt = orderRepo_->findById(orderId);
    if (!orderOpt) throw std::runtime_error("Order not found: " + orderId);
    Order& order = *orderOpt;

    auto sampleOpt = sampleRepo_->findById(order.getSampleId());
    if (!sampleOpt) throw std::runtime_error("Sample not found: " + order.getSampleId());
    Sample& sample = *sampleOpt;

    if (sample.hasEnoughStock(order.getQuantity())) {
        orderRepo_->updateStatus(orderId, OrderStatus::CONFIRMED);
        sampleRepo_->updateStock(sample.getSampleId(), sample.getStock() - order.getQuantity());
    } else {
        int shortfall = sample.getShortfall(order.getQuantity());
        int actualQty = ProductionController::calcActualProductionQty(shortfall, sample.getYieldRate());
        double totalTime = ProductionController::calcTotalProductionTime(
            sample.getAvgProductionTime(), actualQty);

        ProductionJob job(0, orderId, actualQty, totalTime, DateTimeUtil::now());
        prodQueueRepo_->enqueue(job);
        orderRepo_->updateStatus(orderId, OrderStatus::PRODUCING);
    }
}

void OrderController::rejectOrder(const std::string& orderId) {
    auto orderOpt = orderRepo_->findById(orderId);
    if (!orderOpt) throw std::runtime_error("Order not found: " + orderId);
    orderRepo_->updateStatus(orderId, OrderStatus::REJECTED);
}
