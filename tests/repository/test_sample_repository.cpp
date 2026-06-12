#include <gtest/gtest.h>
#include "repository/SampleRepository.h"
#include "util/DBManager.h"

class SampleRepoTest : public ::testing::Test {
protected:
    void SetUp() override {
        DBManager::getInstance().initialize(":memory:");
        repo = std::make_unique<SampleRepository>(DBManager::getInstance().getDB());
    }
    void TearDown() override {
        repo.reset();
        DBManager::getInstance().close();
    }
    std::unique_ptr<SampleRepository> repo;
};

TEST_F(SampleRepoTest, SaveAndFindById) {
    Sample s("S-001", "AlphaX", 10.0, 0.92, 100);
    repo->save(s);
    auto result = repo->findById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ("AlphaX", result->getName());
    EXPECT_EQ(100, result->getStock());
}

TEST_F(SampleRepoTest, FindById_NotFound) {
    auto result = repo->findById("S-999");
    EXPECT_FALSE(result.has_value());
}

TEST_F(SampleRepoTest, FindAll) {
    repo->save(Sample("S-001", "A", 10.0, 0.9, 50));
    repo->save(Sample("S-002", "B", 5.0,  0.85, 30));
    auto all = repo->findAll();
    EXPECT_EQ(2u, all.size());
}

TEST_F(SampleRepoTest, UpdateStock) {
    repo->save(Sample("S-001", "AlphaX", 10.0, 0.92, 100));
    repo->updateStock("S-001", 200);
    auto result = repo->findById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(200, result->getStock());
}

TEST_F(SampleRepoTest, ExistsById_True) {
    repo->save(Sample("S-001", "AlphaX", 10.0, 0.92, 100));
    EXPECT_TRUE(repo->existsById("S-001"));
}

TEST_F(SampleRepoTest, ExistsById_False) {
    EXPECT_FALSE(repo->existsById("S-999"));
}

TEST_F(SampleRepoTest, FindByName) {
    repo->save(Sample("S-001", "AlphaX", 10.0, 0.92, 100));
    auto result = repo->findByName("AlphaX");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ("S-001", result->getSampleId());
}

TEST_F(SampleRepoTest, Remove) {
    repo->save(Sample("S-001", "AlphaX", 10.0, 0.92, 100));
    repo->remove("S-001");
    EXPECT_FALSE(repo->existsById("S-001"));
}
