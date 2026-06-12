#include "view/OrderView.h"
#include <iostream>
#include <limits>

OrderView::OrderInput OrderView::getOrderInput() {
    OrderInput in{};
    std::cout << "Sample ID: ";
    std::getline(std::cin, in.sampleId);
    std::cout << "Customer Name: ";
    std::getline(std::cin, in.customerName);

    while (true) {
        std::cout << "Quantity (>0): ";
        std::cin >> in.quantity;
        if (!std::cin.fail() && in.quantity > 0) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid value. Quantity must be greater than 0.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return in;
}

bool OrderView::confirmOrder(const OrderInput& in) {
    std::cout << "Sample:" << in.sampleId
              << " Customer:" << in.customerName
              << " Qty:" << in.quantity << "\n";
    std::cout << "Confirm? [Y/N]: ";
    char c;
    std::cin >> c;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return (c == 'Y' || c == 'y');
}

void OrderView::showOrderPlaced(const std::string& orderId) {
    std::cout << "Order placed: " << orderId << "\n";
}

void OrderView::showError(const std::string& msg) {
    std::cout << "[Error] " << msg << "\n";
}
