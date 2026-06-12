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
};

TEST_F(DBManagerTest, InitializeInMemorySucceeds) {
    EXPECT_NE(nullptr, DBManager::getInstance().getDB());
}

TEST_F(DBManagerTest, CreateTablesSucceeds) {
    // Tables created in initialize(); should be queryable
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
