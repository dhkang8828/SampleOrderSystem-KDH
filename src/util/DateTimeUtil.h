#pragma once
#include <string>

class DateTimeUtil {
public:
    DateTimeUtil() = delete;
    static std::string now();
    static std::string today();
};
