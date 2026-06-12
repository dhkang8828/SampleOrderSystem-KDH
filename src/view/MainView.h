#pragma once

class MainView {
public:
    static constexpr int RESET_CMD = 99;
    static constexpr int DUMMY_CMD = 98;

    void showHeader();
    void showSummary(int sampleCount, int totalStock, int orderCount, int queueCount);
    void showMenu();
    int getMenuChoice();  // returns 0~6 or RESET_CMD
};
