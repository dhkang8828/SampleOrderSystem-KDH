#pragma once
#include <string>
#include <sqlite3.h>

class DBManager {
public:
    static DBManager& getInstance();

    void initialize(const std::string& dbPath);
    void close();
    sqlite3* getDB() const;
    void resetAllData();

    DBManager(const DBManager&) = delete;
    DBManager& operator=(const DBManager&) = delete;

private:
    DBManager() = default;
    ~DBManager();

    void createTables();

    sqlite3* db_ = nullptr;
};
