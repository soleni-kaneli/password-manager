#include <gtest/gtest.h>

#include "generator.hpp"

using pwdgen::GeneratorOptions;
using pwdgen::PasswordGenerator;

TEST(PasswordGenerator, ProducesCorrectLength) {
    GeneratorOptions opts;
    opts.length = 16;
    PasswordGenerator gen(opts);

    EXPECT_EQ(gen.generate().size(), 16u);
}

TEST(PasswordGenerator, RespectsDigitsOnlyOption) {
    GeneratorOptions opts;
    opts.length = 50;
    opts.useUpper = false;
    opts.useLower = false;
    opts.useSymbols = false;
    opts.useDigits = true;
    PasswordGenerator gen(opts);

    std::string password = gen.generate();
    for (char c : password) {
        EXPECT_TRUE(std::isdigit(static_cast<unsigned char>(c)));
    }
}

TEST(PasswordGenerator, ThrowsOnZeroLength) {
    GeneratorOptions opts;
    opts.length = 0;
    EXPECT_THROW(PasswordGenerator gen(opts), std::invalid_argument);
}

TEST(PasswordGenerator, ThrowsWhenNoCharsetSelected) {
    GeneratorOptions opts;
    opts.useUpper = false;
    opts.useLower = false;
    opts.useDigits = false;
    opts.useSymbols = false;
    EXPECT_THROW(PasswordGenerator gen(opts), std::invalid_argument);
}

TEST(PasswordGenerator, ExcludesAmbiguousCharacters) {
    GeneratorOptions opts;
    opts.length = 200;  // достатъчно голямо, за да покрие всички символи
    opts.excludeAmbiguous = true;
    PasswordGenerator gen(opts);

    std::string password = gen.generate();
    const std::string ambiguous = "0O1lI";
    for (char c : password) {
        EXPECT_EQ(ambiguous.find(c), std::string::npos);
    }
}

TEST(PasswordGenerator, GeneratesDifferentPasswords) {
    GeneratorOptions opts;
    opts.length = 20;
    PasswordGenerator gen(opts);

    // Не е 100% гаранция, но за 20 символа шансът за колизия е нищожен
    EXPECT_NE(gen.generate(), gen.generate());
}
