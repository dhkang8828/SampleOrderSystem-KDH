#include "view/MainView.h"
#include <iostream>
#include <string>
#include <stdexcept>

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
    std::string line;
    while (true) {
        if (!std::getline(std::cin, line)) {
            return 0;  // EOF -> exit gracefully
        }
        if (line == "RESET") return RESET_CMD;
        if (line == "DUMMY") return DUMMY_CMD;
        try {
            int val = std::stoi(line);
            if (val >= 0 && val <= 6) return val;
        } catch (...) {}
        std::cout << "Invalid choice. Enter 0~6 (or RESET): ";
    }
}
