#include "repository/SampleRepository.h"
#include <stdexcept>

SampleRepository::SampleRepository(sqlite3* db) : db_(db) {}

Sample SampleRepository::rowToSample(sqlite3_stmt* stmt) {
    std::string id   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    double apt  = sqlite3_column_double(stmt, 2);
    double yr   = sqlite3_column_double(stmt, 3);
    int stock   = sqlite3_column_int(stmt, 4);
    return Sample(id, name, apt, yr, stock);
}

void SampleRepository::save(const Sample& s) {
    const char* sql =
        "INSERT INTO samples (sample_id, name, avg_prod_time, yield_rate, stock) "
        "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, s.getSampleId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, s.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, s.getAvgProductionTime());
    sqlite3_bind_double(stmt, 4, s.getYieldRate());
    sqlite3_bind_int(stmt, 5, s.getStock());
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::optional<Sample> SampleRepository::findById(const std::string& id) {
    const char* sql = "SELECT sample_id, name, avg_prod_time, yield_rate, stock "
                      "FROM samples WHERE sample_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto s = rowToSample(stmt);
        sqlite3_finalize(stmt);
        return s;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<Sample> SampleRepository::findAll() {
    const char* sql = "SELECT sample_id, name, avg_prod_time, yield_rate, stock FROM samples;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    std::vector<Sample> result;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        result.push_back(rowToSample(stmt));
    }
    sqlite3_finalize(stmt);
    return result;
}

void SampleRepository::update(const Sample& s) {
    const char* sql =
        "UPDATE samples SET name=?, avg_prod_time=?, yield_rate=?, stock=? "
        "WHERE sample_id=?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, s.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, s.getAvgProductionTime());
    sqlite3_bind_double(stmt, 3, s.getYieldRate());
    sqlite3_bind_int(stmt, 4, s.getStock());
    sqlite3_bind_text(stmt, 5, s.getSampleId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void SampleRepository::remove(const std::string& id) {
    const char* sql = "DELETE FROM samples WHERE sample_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::optional<Sample> SampleRepository::findByName(const std::string& name) {
    const char* sql = "SELECT sample_id, name, avg_prod_time, yield_rate, stock "
                      "FROM samples WHERE name = ?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto s = rowToSample(stmt);
        sqlite3_finalize(stmt);
        return s;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

void SampleRepository::updateStock(const std::string& sampleId, int newStock) {
    const char* sql = "UPDATE samples SET stock=? WHERE sample_id=?;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, newStock);
    sqlite3_bind_text(stmt, 2, sampleId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

bool SampleRepository::existsById(const std::string& id) {
    return findById(id).has_value();
}

int SampleRepository::getNextSequence() {
    const char* sql = "SELECT COUNT(*) FROM samples;";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count + 1;
}
