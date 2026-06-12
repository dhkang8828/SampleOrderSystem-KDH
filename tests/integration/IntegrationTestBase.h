#pragma once
#include <gtest/gtest.h>
#include <memory>
#include "util/DBManager.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionQueueRepository.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/ReleaseController.h"

class IntegrationTestBase : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
        sqlite3* db = DBManager::getInstance().getDB();

        sampleRepo    = std::make_unique<SampleRepository>(db);
        orderRepo     = std::make_unique<OrderRepository>(db);
        prodQueueRepo = std::make_unique<ProductionQueueRepository>(db);

        sampleCtrl  = std::make_unique<SampleController>(sampleRepo.get());
        orderCtrl   = std::make_unique<OrderController>(
                          sampleRepo.get(), orderRepo.get(), prodQueueRepo.get());
        releaseCtrl = std::make_unique<ReleaseController>(
                          orderRepo.get(), sampleRepo.get(), prodQueueRepo.get());
    }

    void TearDown() override {
        releaseCtrl.reset();
        orderCtrl.reset();
        sampleCtrl.reset();
        prodQueueRepo.reset();
        orderRepo.reset();
        sampleRepo.reset();
        DBManager::getInstance().close();
    }

    // Helpers
    std::string registerSample(const std::string& name,
                                double avgTime, double yieldRate, int stock) {
        return sampleCtrl->registerSample(name, avgTime, yieldRate, stock);
    }

    // Complete the first job in the queue and return its orderId
    std::string completeFirstJob() {
        auto job = prodQueueRepo->peek();
        if (!job) return "";
        std::string oid = job->getOrderId();
        int qty = job->getActualProductionQty();
        prodQueueRepo->remove(std::to_string(job->getQueueId()));
        releaseCtrl->completeProduction(oid, qty);
        return oid;
    }

    std::unique_ptr<SampleRepository>          sampleRepo;
    std::unique_ptr<OrderRepository>           orderRepo;
    std::unique_ptr<ProductionQueueRepository> prodQueueRepo;
    std::unique_ptr<SampleController>          sampleCtrl;
    std::unique_ptr<OrderController>           orderCtrl;
    std::unique_ptr<ReleaseController>         releaseCtrl;
};
