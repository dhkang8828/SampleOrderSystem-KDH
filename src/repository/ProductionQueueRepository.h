#pragma once
#include "repository/IRepository.h"
#include "repository/IScheduler.h"
#include "model/ProductionJob.h"
#include <sqlite3.h>
#include <optional>
#include <vector>
#include <string>

class ProductionQueueRepository
    : public IRepository<ProductionJob>
    , public IScheduler {
public:
    explicit ProductionQueueRepository(sqlite3* db);

    // IRepository<ProductionJob>
    void save(const ProductionJob& job) override;
    std::optional<ProductionJob> findById(const std::string& id) override;
    std::vector<ProductionJob> findAll() override;
    void update(const ProductionJob& job) override;
    void remove(const std::string& id) override;

    // IScheduler (FIFO)
    void enqueue(const ProductionJob& job) override;
    std::optional<ProductionJob> dequeue() override;
    std::optional<ProductionJob> peek() override;
    bool isEmpty() const override;

private:
    sqlite3* db_;
    ProductionJob rowToJob(sqlite3_stmt* stmt);
};
