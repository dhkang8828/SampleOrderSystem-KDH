#include <gtest/gtest.h>
#include "util/DBManager.h"
#include <filesystem>

class DBManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
    }
    void TearDown() override {
        DBManager::getInstance().close();
    }

    int countRows(const char* table) {
        sqlite3* db = DBManager::getInstance().getDB();
        std::string sql = std::string("SELECT COUNT(*) FROM ") + table;
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        sqlite3_step(stmt);
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return count;
    }
};

TEST_F(DBManagerTest, InitializeInMemorySucceeds) {
    EXPECT_NE(nullptr, DBManager::getInstance().getDB());
}

TEST_F(DBManagerTest, CreateTablesSucceeds) {
    sqlite3* db = DBManager::getInstance().getDB();
    ASSERT_NE(nullptr, db);

    const char* sql = "SELECT name FROM sqlite_master WHERE type='table';";
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    EXPECT_EQ(SQLITE_OK, rc);
    sqlite3_finalize(stmt);
}

TEST_F(DBManagerTest, IsSingletonInstance) {
    DBManager& a = DBManager::getInstance();
    DBManager& b = DBManager::getInstance();
    EXPECT_EQ(&a, &b);
}

TEST_F(DBManagerTest, ResetAllData_ClearsAllTables) {
    sqlite3* db = DBManager::getInstance().getDB();

    // Insert sample data into all 3 tables
    sqlite3_exec(db,
        "INSERT INTO samples VALUES ('S-001','TestSample',10.0,0.9,100);",
        nullptr, nullptr, nullptr);
    sqlite3_exec(db,
        "INSERT INTO orders VALUES "
        "('ORD-20260612-0001','S-001','CustomerA',10,'RESERVED','2026-06-12T00:00:00');",
        nullptr, nullptr, nullptr);
    sqlite3_exec(db,
        "INSERT INTO production_queue(order_id,actual_prod_qty,total_prod_time,enqueued_at) "
        "VALUES ('ORD-20260612-0001',12,120.0,'2026-06-12T00:00:00');",
        nullptr, nullptr, nullptr);

    ASSERT_EQ(1, countRows("samples"));
    ASSERT_EQ(1, countRows("orders"));
    ASSERT_EQ(1, countRows("production_queue"));

    DBManager::getInstance().resetAllData();

    EXPECT_EQ(0, countRows("samples"));
    EXPECT_EQ(0, countRows("orders"));
    EXPECT_EQ(0, countRows("production_queue"));
}

TEST_F(DBManagerTest, ResetAllData_EmptyDB_DoesNotThrow) {
    EXPECT_NO_THROW(DBManager::getInstance().resetAllData());
}
