#include "repository/OrderRepository.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

OrderRepository::OrderRepository(sqlite3* db) : db_(db) {}

std::string OrderRepository::statusToString(OrderStatus s) {
    switch (s) {
        case OrderStatus::RESERVED:  return "RESERVED";
        case OrderStatus::REJECTED:  return "REJECTED";
        case OrderStatus::PRODUCING: return "PRODUCING";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::RELEASED:  return "RELEASED";
    }
    return "RESERVED";
}

OrderStatus OrderRepository::stringToStatus(const std::string& s) {
    if (s == "REJECTED")  return OrderStatus::REJECTED;
    if (s == "PRODUCING") return OrderStatus::PRODUCING;
    if (s == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (s == "RELEASED")  return OrderStatus::RELEASED;
    return OrderStatus::RESERVED;
}

Order OrderRepository::rowToOrder(sqlite3_stmt* stmt) {
    std::string orderId      = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    std::string sampleId     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    std::string customerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    int quantity             = sqlite3_column_int(stmt, 3);
    std::string statusStr    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    std::string createdAt    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

    Order o(orderId, sampleId, customerName, quantity, createdAt);
    o.setStatus(stringToStatus(statusStr));
    return o;
}

void OrderRepository::save(const Order& o) {
    const char* sql =
        "INSERT INTO orders (order_id, sample_id, customer_name, quantity, status, created_at) "
        "VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, o.getOrderId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, o.getSampleId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, o.getCustomerName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, o.getQuantity());
    sqlite3_bind_text(stmt, 5, statusToString(o.getStatus()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, o.getCreatedAt().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::optional<Order> OrderRepository::findById(const std::string& id) {
    const char* sql =
        "SELECT order_id, sample_id, customer_name, quantity, status, created_at "
        "FROM orders WHERE order_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto o = rowToOrder(stmt);
        sqlite3_finalize(stmt);
        return o;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<Order> OrderRepository::findAll() {
    const char* sql =
        "SELECT order_id, sample_id, customer_name, quantity, status, created_at FROM orders;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    std::vector<Order> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back(rowToOrder(stmt));
    }
    sqlite3_finalize(stmt);
    return result;
}

void OrderRepository::update(const Order& o) {
    const char* sql =
        "UPDATE orders SET sample_id=?, customer_name=?, quantity=?, status=?, created_at=? "
        "WHERE order_id=?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, o.getSampleId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, o.getCustomerName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, o.getQuantity());
    sqlite3_bind_text(stmt, 4, statusToString(o.getStatus()).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, o.getCreatedAt().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, o.getOrderId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void OrderRepository::remove(const std::string& id) {
    const char* sql = "DELETE FROM orders WHERE order_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::vector<Order> OrderRepository::findByStatus(OrderStatus status) {
    const char* sql =
        "SELECT order_id, sample_id, customer_name, quantity, status, created_at "
        "FROM orders WHERE status = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, statusToString(status).c_str(), -1, SQLITE_TRANSIENT);
    std::vector<Order> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back(rowToOrder(stmt));
    }
    sqlite3_finalize(stmt);
    return result;
}

void OrderRepository::updateStatus(const std::string& orderId, OrderStatus status) {
    const char* sql = "UPDATE orders SET status=? WHERE order_id=?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, statusToString(status).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, orderId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

int OrderRepository::countByStatus(OrderStatus status) {
    const char* sql = "SELECT COUNT(*) FROM orders WHERE status = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, statusToString(status).c_str(), -1, SQLITE_TRANSIENT);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

int OrderRepository::generateNextOrderSequence(const std::string& dateStr) {
    const char* sql =
        "SELECT COUNT(*) FROM orders WHERE order_id LIKE ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    std::string pattern = "ORD-" + dateStr + "-%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count + 1;
}
