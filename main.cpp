#include <iostream>
#include <map>
#include <string>

#include "util/DBManager.h"
#include "util/ConsoleUtil.h"
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "repository/ProductionQueueRepository.h"
#include "controller/SampleController.h"
#include "controller/OrderController.h"
#include "controller/ReleaseController.h"
#include "view/MainView.h"
#include "view/SampleView.h"
#include "view/OrderView.h"
#include "view/ApprovalView.h"
#include "view/MonitoringView.h"
#include "view/ProductionView.h"
#include "view/ReleaseView.h"

int main() {
    DBManager::getInstance().initialize("data/sample_order.db");
    sqlite3* db = DBManager::getInstance().getDB();

    SampleRepository      sampleRepo(db);
    OrderRepository       orderRepo(db);
    ProductionQueueRepository prodQueueRepo(db);

    SampleController  sampleCtrl(&sampleRepo);
    OrderController   orderCtrl(&sampleRepo, &orderRepo, &prodQueueRepo);
    ReleaseController releaseCtrl(&orderRepo, &sampleRepo, &prodQueueRepo);

    MainView      mainView;
    SampleView    sampleView;
    OrderView     orderView;
    ApprovalView  approvalView;
    MonitoringView monitorView;
    ProductionView productionView;
    ReleaseView   releaseView;

    bool running = true;
    while (running) {
        ConsoleUtil::clearScreen();

        auto samples = sampleCtrl.listSamples();
        int totalStock = 0;
        for (const auto& s : samples) totalStock += s.getStock();
        int orderCount = static_cast<int>(orderRepo.findAll().size());
        int queueCount = static_cast<int>(prodQueueRepo.findAll().size());

        mainView.showHeader();
        mainView.showSummary(static_cast<int>(samples.size()), totalStock, orderCount, queueCount);
        mainView.showMenu();
        int choice = mainView.getMenuChoice();

        switch (choice) {
        case 0:
            running = false;
            std::cout << "Goodbye.\n";
            break;

        case 1: { // Sample management
            bool inSample = true;
            while (inSample) {
                ConsoleUtil::clearScreen();
                sampleView.showMenu();
                int sc = sampleView.getMenuChoice();
                if (sc == 0) { inSample = false; break; }
                if (sc == 1) {
                    auto in = sampleView.getSampleInput();
                    std::string id = sampleCtrl.registerSample(in.name, in.avgProdTime, in.yieldRate, in.stock);
                    std::cout << "Registered: " << id << "\n";
                } else if (sc == 2) {
                    sampleView.showSampleList(sampleCtrl.listSamples());
                } else if (sc == 3) {
                    std::string id = sampleView.getSampleId();
                    auto s = sampleCtrl.findSample(id);
                    if (s) sampleView.showSample(*s);
                    else   sampleView.showNotFound(id);
                }
                ConsoleUtil::pressEnterToContinue();
            }
            break;
        }

        case 2: { // Place order
            auto in = orderView.getOrderInput();
            if (orderView.confirmOrder(in)) {
                try {
                    std::string oid = orderCtrl.placeOrder(in.sampleId, in.customerName, in.quantity);
                    orderView.showOrderPlaced(oid);
                } catch (const std::exception& e) {
                    orderView.showError(e.what());
                }
            }
            ConsoleUtil::pressEnterToContinue();
            break;
        }

        case 3: { // Approve/Reject
            auto reserved = orderRepo.findByStatus(OrderStatus::RESERVED);
            approvalView.showReservedOrders(reserved);
            if (!reserved.empty()) {
                std::string oid = approvalView.selectOrderId();
                int action = approvalView.getApprovalAction();
                try {
                    if (action == 1) {
                        orderCtrl.approveOrder(oid);
                        approvalView.showResult(oid, "APPROVED");
                    } else if (action == 2) {
                        orderCtrl.rejectOrder(oid);
                        approvalView.showResult(oid, "REJECTED");
                    }
                } catch (const std::exception& e) {
                    std::cout << "[Error] " << e.what() << "\n";
                }
            }
            ConsoleUtil::pressEnterToContinue();
            break;
        }

        case 4: { // Monitoring
            std::map<std::string, int> counts;
            counts["RESERVED"]  = orderRepo.countByStatus(OrderStatus::RESERVED);
            counts["CONFIRMED"] = orderRepo.countByStatus(OrderStatus::CONFIRMED);
            counts["PRODUCING"] = orderRepo.countByStatus(OrderStatus::PRODUCING);
            counts["RELEASED"]  = orderRepo.countByStatus(OrderStatus::RELEASED);
            monitorView.showOrderCounts(counts);

            std::map<std::string, int> pendingQty;
            for (const auto& o : orderRepo.findByStatus(OrderStatus::RESERVED))
                pendingQty[o.getSampleId()] += o.getQuantity();
            for (const auto& o : orderRepo.findByStatus(OrderStatus::PRODUCING))
                pendingQty[o.getSampleId()] += o.getQuantity();
            monitorView.showStockStatus(sampleCtrl.listSamples(), pendingQty);
            ConsoleUtil::pressEnterToContinue();
            break;
        }

        case 5: { // Production queue
            auto jobs = prodQueueRepo.findAll();
            productionView.showQueue(jobs);
            if (!jobs.empty()) {
                std::string oid = productionView.selectOrderId();
                if (!oid.empty()) {
                    try {
                        auto jobOpt = orderRepo.findById(oid);
                        if (jobOpt) {
                            auto queueJobs = prodQueueRepo.findAll();
                            int producedQty = 0;
                            for (const auto& j : queueJobs) {
                                if (j.getOrderId() == oid) {
                                    producedQty = j.getActualProductionQty();
                                    prodQueueRepo.remove(std::to_string(j.getQueueId()));
                                    break;
                                }
                            }
                            releaseCtrl.completeProduction(oid, producedQty);
                            productionView.showCompleted(oid);
                        }
                    } catch (const std::exception& e) {
                        std::cout << "[Error] " << e.what() << "\n";
                    }
                }
            }
            ConsoleUtil::pressEnterToContinue();
            break;
        }

        case 6: { // Release
            auto confirmed = orderRepo.findByStatus(OrderStatus::CONFIRMED);
            releaseView.showConfirmedOrders(confirmed);
            if (!confirmed.empty()) {
                std::string oid = releaseView.selectOrderId();
                try {
                    releaseCtrl.releaseOrder(oid);
                    releaseView.showReleased(oid);
                } catch (const std::exception& e) {
                    std::cout << "[Error] " << e.what() << "\n";
                }
            }
            ConsoleUtil::pressEnterToContinue();
            break;
        }

        case MainView::RESET_CMD: { // hidden: type "RESET" at main menu
            std::cout << "\n[WARNING] This will permanently delete ALL samples, orders, and production data.\n";
            std::cout << "Type CONFIRM to proceed, anything else to cancel: ";
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm == "CONFIRM") {
                DBManager::getInstance().resetAllData();
                std::cout << "All data has been reset.\n";
            } else {
                std::cout << "Reset cancelled.\n";
            }
            ConsoleUtil::pressEnterToContinue();
            break;
        }

        default:
            break;
        }
    }

    DBManager::getInstance().close();
    return 0;
}
