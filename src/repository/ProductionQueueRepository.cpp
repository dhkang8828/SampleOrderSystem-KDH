#include "repository/ProductionQueueRepository.h"
#include <stdexcept>
#include <string>

ProductionQueueRepository::ProductionQueueRepository(sqlite3* db) : db_(db) {}

ProductionJob ProductionQueueRepository::rowToJob(sqlite3_stmt* stmt) {
    int queueId      = sqlite3_column_int(stmt, 0);
    std::string oid  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    int qty          = sqlite3_column_int(stmt, 2);
    double time      = sqlite3_column_double(stmt, 3);
    std::string enq  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
    return ProductionJob(queueId, oid, qty, time, enq);
}

void ProductionQueueRepository::save(const ProductionJob& job) {
    enqueue(job);
}

std::optional<ProductionJob> ProductionQueueRepository::findById(const std::string& id) {
    const char* sql =
        "SELECT queue_id, order_id, actual_prod_qty, total_prod_time, enqueued_at "
        "FROM production_queue WHERE queue_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, std::stoi(id));
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto j = rowToJob(stmt);
        sqlite3_finalize(stmt);
        return j;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<ProductionJob> ProductionQueueRepository::findAll() {
    const char* sql =
        "SELECT queue_id, order_id, actual_prod_qty, total_prod_time, enqueued_at "
        "FROM production_queue ORDER BY queue_id ASC;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    std::vector<ProductionJob> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back(rowToJob(stmt));
    }
    sqlite3_finalize(stmt);
    return result;
}

void ProductionQueueRepository::update(const ProductionJob& job) {
    const char* sql =
        "UPDATE production_queue SET order_id=?, actual_prod_qty=?, total_prod_time=?, enqueued_at=? "
        "WHERE queue_id=?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, job.getOrderId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, job.getActualProductionQty());
    sqlite3_bind_double(stmt, 3, job.getTotalProductionTime());
    sqlite3_bind_text(stmt, 4, job.getEnqueuedAt().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 5, job.getQueueId());
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void ProductionQueueRepository::remove(const std::string& id) {
    const char* sql = "DELETE FROM production_queue WHERE queue_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, std::stoi(id));
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void ProductionQueueRepository::enqueue(const ProductionJob& job) {
    const char* sql =
        "INSERT INTO production_queue (order_id, actual_prod_qty, total_prod_time, enqueued_at) "
        "VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, job.getOrderId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, job.getActualProductionQty());
    sqlite3_bind_double(stmt, 3, job.getTotalProductionTime());
    sqlite3_bind_text(stmt, 4, job.getEnqueuedAt().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::optional<ProductionJob> ProductionQueueRepository::dequeue() {
    auto front = peek();
    if (!front) return std::nullopt;
    remove(std::to_string(front->getQueueId()));
    return front;
}

std::optional<ProductionJob> ProductionQueueRepository::peek() {
    const char* sql =
        "SELECT queue_id, order_id, actual_prod_qty, total_prod_time, enqueued_at "
        "FROM production_queue ORDER BY queue_id ASC LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto j = rowToJob(stmt);
        sqlite3_finalize(stmt);
        return j;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

bool ProductionQueueRepository::isEmpty() const {
    const char* sql = "SELECT COUNT(*) FROM production_queue;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count == 0;
}
