#pragma once
#include <iostream>
#include <string>

namespace ConsoleUtil {

void clearScreen();
void pressEnterToContinue();

// Core validation logic - takes explicit streams so it is unit-testable.
// Loops until an integer in [min, max] is read; prints errMsg on bad input.
int readValidInt(std::istream& in, std::ostream& out,
                 int min, int max, const std::string& errMsg);

// Convenience wrapper using std::cin / std::cout.
int readMenuChoice(int min, int max);

} // namespace ConsoleUtil
