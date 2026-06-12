#pragma once
#include "repository/IRepository.h"
#include "model/Sample.h"
#include <sqlite3.h>
#include <optional>
#include <vector>
#include <string>

class SampleRepository : public IRepository<Sample> {
public:
    explicit SampleRepository(sqlite3* db);

    void save(const Sample& sample) override;
    std::optional<Sample> findById(const std::string& id) override;
    std::vector<Sample> findAll() override;
    void update(const Sample& sample) override;
    void remove(const std::string& id) override;

    std::optional<Sample> findByName(const std::string& name);
    void updateStock(const std::string& sampleId, int newStock);
    bool existsById(const std::string& id);

    int getNextSequence();

private:
    sqlite3* db_;
    Sample rowToSample(sqlite3_stmt* stmt);
};
