#include <iostream>
#include <string>

#include "generator.hpp"

namespace {

void printUsage(const char* programName) {
    std::cout << "Употреба: " << programName << " [опции]\n"
              << "  -l <брой>   Дължина на паролата (по подразбиране 12)\n"
              << "  --no-upper  Без главни букви\n"
              << "  --no-lower  Без малки букви\n"
              << "  --no-digits Без цифри\n"
              << "  --no-symbols Без символи\n"
              << "  -h          Показва тази помощ\n";
}

}  // namespace

int main(int argc, char** argv) {
    pwdgen::GeneratorOptions options;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-l" && i + 1 < argc) {
            options.length = std::stoi(argv[++i]);
        } else if (arg == "--no-upper") {
            options.useUpper = false;
        } else if (arg == "--no-lower") {
            options.useLower = false;
        } else if (arg == "--no-digits") {
            options.useDigits = false;
        } else if (arg == "--no-symbols") {
            options.useSymbols = false;
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
    }

    try {
        pwdgen::PasswordGenerator generator(options);
        std::cout << generator.generate() << "\n";
    } catch (const std::exception& ex) {
        std::cerr << "Грешка: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
