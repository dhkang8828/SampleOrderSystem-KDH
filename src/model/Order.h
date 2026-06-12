#pragma once
#include <string>

enum class OrderStatus {
    RESERVED,
    REJECTED,
    PRODUCING,
    CONFIRMED,
    RELEASED
};

class Order {
public:
    Order(const std::string& orderId, const std::string& sampleId,
          const std::string& customerName, int quantity,
          const std::string& createdAt);

    const std::string& getOrderId() const { return orderId_; }
    const std::string& getSampleId() const { return sampleId_; }
    const std::string& getCustomerName() const { return customerName_; }
    int getQuantity() const { return quantity_; }
    OrderStatus getStatus() const { return status_; }
    const std::string& getCreatedAt() const { return createdAt_; }

    void setStatus(OrderStatus status) { status_ = status; }

    bool isReserved()  const { return status_ == OrderStatus::RESERVED; }
    bool isConfirmed() const { return status_ == OrderStatus::CONFIRMED; }
    bool isProducing() const { return status_ == OrderStatus::PRODUCING; }
    bool isReleased()  const { return status_ == OrderStatus::RELEASED; }
    bool isRejected()  const { return status_ == OrderStatus::REJECTED; }

private:
    std::string orderId_;
    std::string sampleId_;
    std::string customerName_;
    int quantity_;
    OrderStatus status_;
    std::string createdAt_;
};
