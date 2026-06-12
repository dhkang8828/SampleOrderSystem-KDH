#pragma once
#include <string>

enum class StockStatus {
    SUFFICIENT,
    SHORTAGE,
    DEPLETED
};

class Sample {
public:
    Sample(const std::string& sampleId, const std::string& name,
           double avgProductionTime, double yieldRate, int stock);

    const std::string& getSampleId() const { return sampleId_; }
    const std::string& getName() const { return name_; }
    double getAvgProductionTime() const { return avgProductionTime_; }
    double getYieldRate() const { return yieldRate_; }
    int getStock() const { return stock_; }
    void setStock(int stock) { stock_ = stock; }

    bool hasEnoughStock(int quantity) const;
    int getShortfall(int quantity) const;
    StockStatus getStockStatus(int quantity) const;

private:
    std::string sampleId_;
    std::string name_;
    double avgProductionTime_;
    double yieldRate_;
    int stock_;
};
