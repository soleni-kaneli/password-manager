#include "generator.hpp"

#include <random>
#include <algorithm>

namespace pwdgen {

namespace {
const std::string kUpper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string kLower = "abcdefghijklmnopqrstuvwxyz";
const std::string kDigits = "0123456789";
const std::string kSymbols = "!@#$%^&*()-_=+[]{}";
}  // namespace

PasswordGenerator::PasswordGenerator(const GeneratorOptions& options)
    : options_(options) {
    if (options_.length <= 0) {
        throw std::invalid_argument("Password length must be positive");
    }
    if (!options_.useUpper && !options_.useLower &&
        !options_.useDigits && !options_.useSymbols) {
        throw std::invalid_argument("At least one character set must be enabled");
    }
}

std::string PasswordGenerator::buildCharacterPool() const {
    std::string pool;
    if (options_.useUpper) pool += kUpper;
    if (options_.useLower) pool += kLower;
    if (options_.useDigits) pool += kDigits;
    if (options_.useSymbols) pool += kSymbols;

    if (options_.excludeAmbiguous) {
        const std::string ambiguous = "0O1lI";
        pool.erase(std::remove_if(pool.begin(), pool.end(),
                                   [&](char c) {
                                       return ambiguous.find(c) != std::string::npos;
                                   }),
                   pool.end());
    }
    return pool;
}

std::string PasswordGenerator::generate() const {
    const std::string pool = buildCharacterPool();

    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<std::size_t> dist(0, pool.size() - 1);

    std::string password;
    password.reserve(options_.length);
    for (int i = 0; i < options_.length; ++i) {
        password += pool[dist(engine)];
    }
    return password;
}

}  // namespace pwdgen
