#include "util/DateTimeUtil.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstring>

std::string DateTimeUtil::now() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

std::string DateTimeUtil::today() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d");
    return oss.str();
}

double DateTimeUtil::elapsedMinutesSince(const std::string& isoDatetime) {
    std::tm tm{};
    std::istringstream ss(isoDatetime);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    tm.tm_isdst = -1;
    std::time_t then = std::mktime(&tm);
    std::time_t nowT = std::time(nullptr);
    return std::difftime(nowT, then) / 60.0;
}
