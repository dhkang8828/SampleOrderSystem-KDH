#include "view/MainView.h"
#include <iostream>
#include <limits>

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
    int choice = -1;
    std::cin >> choice;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}
