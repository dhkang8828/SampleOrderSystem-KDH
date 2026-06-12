#pragma once
#include "repository/IRepository.h"
#include "model/Order.h"
#include <sqlite3.h>
#include <optional>
#include <vector>
#include <string>

class OrderRepository : public IRepository<Order> {
public:
    explicit OrderRepository(sqlite3* db);

    void save(const Order& order) override;
    std::optional<Order> findById(const std::string& id) override;
    std::vector<Order> findAll() override;
    void update(const Order& order) override;
    void remove(const std::string& id) override;

    std::vector<Order> findByStatus(OrderStatus status);
    void updateStatus(const std::string& orderId, OrderStatus status);
    int countByStatus(OrderStatus status);
    int generateNextOrderSequence(const std::string& dateStr);

private:
    sqlite3* db_;
    Order rowToOrder(sqlite3_stmt* stmt);
    static std::string statusToString(OrderStatus status);
    static OrderStatus stringToStatus(const std::string& s);
};
