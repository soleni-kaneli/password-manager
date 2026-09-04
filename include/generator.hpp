#pragma once

#include <string>
#include <stdexcept>

namespace pwdgen {

// Настройки за генерирането на паролата
struct GeneratorOptions {
    int length = 12;
    bool useUpper = true;
    bool useLower = true;
    bool useDigits = true;
    bool useSymbols = true;
    bool excludeAmbiguous = false;  // Изключва символи като 0/O, 1/l/I
};

class PasswordGenerator {
public:
    explicit PasswordGenerator(const GeneratorOptions& options);

    // Генерира и връща една случайна парола според настройките
    std::string generate() const;

private:
    GeneratorOptions options_;
    std::string buildCharacterPool() const;
};

}  // namespace pwdgen
