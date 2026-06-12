#include "view/SampleView.h"
#include "util/ConsoleUtil.h"
#include <iostream>
#include <iomanip>
#include <limits>

void SampleView::showMenu() {
    std::cout << "\n--- Sample Management ---\n";
    std::cout << "[1] Register  [2] List  [3] Find  [0] Back\n";
    std::cout << "Select: ";
}

int SampleView::getMenuChoice() {
    return ConsoleUtil::readMenuChoice(0, 3);
}

void SampleView::showSampleList(const std::vector<Sample>& samples) {
    if (samples.empty()) {
        std::cout << "No samples registered.\n";
        return;
    }
    std::cout << std::left
              << std::setw(8)  << "ID"
              << std::setw(16) << "Name"
              << std::setw(12) << "AvgTime"
              << std::setw(10) << "Yield"
              << std::setw(8)  << "Stock" << "\n";
    std::cout << std::string(54, '-') << "\n";
    for (const auto& s : samples) {
        std::cout << std::setw(8)  << s.getSampleId()
                  << std::setw(16) << s.getName()
                  << std::setw(12) << s.getAvgProductionTime()
                  << std::setw(10) << s.getYieldRate()
                  << std::setw(8)  << s.getStock() << "\n";
    }
}

void SampleView::showSample(const Sample& s) {
    std::cout << "ID:" << s.getSampleId()
              << " Name:" << s.getName()
              << " Stock:" << s.getStock() << "\n";
}

void SampleView::showNotFound(const std::string& id) {
    std::cout << "Sample not found: " << id << "\n";
}

SampleView::SampleInput SampleView::getSampleInput() {
    SampleInput in{};
    std::cout << "Name: ";
    std::getline(std::cin, in.name);

    while (true) {
        std::cout << "Avg Production Time (min/ea, >0): ";
        std::cin >> in.avgProdTime;
        if (!std::cin.fail() && in.avgProdTime > 0.0) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid value. Must be greater than 0.\n";
    }

    while (true) {
        std::cout << "Yield Rate (0~1): ";
        std::cin >> in.yieldRate;
        if (!std::cin.fail() && in.yieldRate > 0.0 && in.yieldRate <= 1.0) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid value. Must be between 0 (exclusive) and 1 (inclusive).\n";
    }

    while (true) {
        std::cout << "Initial Stock (>=0): ";
        std::cin >> in.stock;
        if (!std::cin.fail() && in.stock >= 0) break;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid value. Must be 0 or greater.\n";
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return in;
}

std::string SampleView::getSampleId() {
    std::string id;
    std::cout << "Sample ID: ";
    std::getline(std::cin, id);
    return id;
}
