#include "util/DBManager.h"
#include <filesystem>
#include <stdexcept>

DBManager& DBManager::getInstance() {
    static DBManager instance;
    return instance;
}

DBManager::~DBManager() {
    close();
}

void DBManager::initialize(const std::string& dbPath) {
    if (dbPath != ":memory:") {
        auto dir = std::filesystem::path(dbPath).parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }
    }

    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        throw std::runtime_error(std::string("Failed to open DB: ") + sqlite3_errmsg(db_));
    }
    createTables();
}

void DBManager::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

sqlite3* DBManager::getDB() const {
    return db_;
}

void DBManager::resetAllData() {
    const char* sql =
        "DELETE FROM production_queue;"
        "DELETE FROM orders;"
        "DELETE FROM samples;";
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string msg = errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to reset data: " + msg);
    }
}

void DBManager::createTables() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS samples ("
        "  sample_id    TEXT PRIMARY KEY,"
        "  name         TEXT NOT NULL,"
        "  avg_prod_time REAL NOT NULL,"
        "  yield_rate   REAL NOT NULL,"
        "  stock        INTEGER NOT NULL DEFAULT 0"
        ");"
        "CREATE TABLE IF NOT EXISTS orders ("
        "  order_id      TEXT PRIMARY KEY,"
        "  sample_id     TEXT NOT NULL,"
        "  customer_name TEXT NOT NULL,"
        "  quantity      INTEGER NOT NULL,"
        "  status        TEXT NOT NULL,"
        "  created_at    TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS production_queue ("
        "  queue_id             INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  order_id             TEXT NOT NULL,"
        "  actual_prod_qty      INTEGER NOT NULL,"
        "  total_prod_time      REAL NOT NULL,"
        "  enqueued_at          TEXT NOT NULL"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string msg = errMsg ? errMsg : "unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("Failed to create tables: " + msg);
    }
}
