#pragma once
#include <string>

class IdGenerator {
public:
    IdGenerator() = delete;
    static std::string generateOrderId(const std::string& dateStr, int sequence);
    static std::string generateSampleId(int sequence);
};
