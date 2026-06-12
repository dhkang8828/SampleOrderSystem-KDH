#include "util/ConsoleUtil.h"
#include <limits>
#include <cstdlib>

namespace ConsoleUtil {

void clearScreen() {
    system("cls");
}

void pressEnterToContinue() {
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

int readValidInt(std::istream& in, std::ostream& out,
                 int min, int max, const std::string& errMsg) {
    int value;
    while (true) {
        if (in >> value) {
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (value >= min && value <= max)
                return value;
        } else {
            in.clear();
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        out << errMsg;
    }
}

int readMenuChoice(int min, int max) {
    return readValidInt(std::cin, std::cout, min, max,
                        "Invalid choice. Enter " + std::to_string(min) +
                        "~" + std::to_string(max) + ": ");
}

} // namespace ConsoleUtil
