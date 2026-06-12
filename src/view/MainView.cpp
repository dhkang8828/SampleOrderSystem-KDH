#include "view/MainView.h"
#include "util/ConsoleUtil.h"
#include <iostream>

void MainView::showHeader() {
    std::cout << "\n========================================\n";
    std::cout << " S-Semi Sample Order Management System \n";
    std::cout << "========================================\n";
}

void MainView::showSummary(int sampleCount, int totalStock, int orderCount, int queueCount) {
    std::cout << "[Status] Samples:" << sampleCount
              << "  Stock:" << totalStock
              << "  Orders:" << orderCount
              << "  Queue:" << queueCount << "\n";
}

void MainView::showMenu() {
    std::cout << "\n[1] Sample Mgmt  [2] Place Order  [3] Approve/Reject\n";
    std::cout << "[4] Monitoring   [5] Prod Queue   [6] Release  [0] Exit\n";
    std::cout << "Select: ";
}

int MainView::getMenuChoice() {
    return ConsoleUtil::readMenuChoice(0, 6);
}
