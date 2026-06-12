#pragma once

class MainView {
public:
    void showHeader();
    void showSummary(int sampleCount, int totalStock, int orderCount, int queueCount);
    void showMenu();
    int getMenuChoice();
};
