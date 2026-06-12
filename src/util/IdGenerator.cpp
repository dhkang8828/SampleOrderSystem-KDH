#include "util/IdGenerator.h"
#include <sstream>
#include <iomanip>

std::string IdGenerator::generateOrderId(const std::string& dateStr, int sequence) {
    std::ostringstream oss;
    oss << "ORD-" << dateStr << "-" << std::setw(4) << std::setfill('0') << sequence;
    return oss.str();
}

std::string IdGenerator::generateSampleId(int sequence) {
    std::ostringstream oss;
    oss << "S-" << std::setw(3) << std::setfill('0') << sequence;
    return oss.str();
}
