#pragma once
#include "model/ProductionJob.h"
#include <optional>

class IScheduler {
public:
    virtual ~IScheduler() = default;
    virtual void enqueue(const ProductionJob& job) = 0;
    virtual std::optional<ProductionJob> dequeue() = 0;
    virtual std::optional<ProductionJob> peek() = 0;
    virtual bool isEmpty() const = 0;
};
