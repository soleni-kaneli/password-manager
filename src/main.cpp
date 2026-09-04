#include <iostream>
#include <string>

#include "generator.hpp"
#include "httplib.h"

namespace {

void printUsage(const char* programName) {
    std::cout << "Употреба:\n"
              << "  " << programName << " [опции]        Генерира една парола в конзолата\n"
              << "  " << programName << " serve [--port <N>]  Стартира HTTP услуга (по подразбиране порт 8080)\n\n"
              << "Опции за CLI режим:\n"
              << "  -l <брой>   Дължина на паролата (по подразбиране 12)\n"
              << "  --no-upper  Без главни букви\n"
              << "  --no-lower  Без малки букви\n"
              << "  --no-digits Без цифри\n"
              << "  --no-symbols Без символи\n"
              << "  -h          Показва тази помощ\n";
}

// Чете GeneratorOptions от query параметрите на HTTP заявката
pwdgen::GeneratorOptions optionsFromRequest(const httplib::Request& req) {
    pwdgen::GeneratorOptions options;
    if (req.has_param("length")) {
        options.length = std::stoi(req.get_param_value("length"));
    }
    if (req.has_param("upper")) {
        options.useUpper = req.get_param_value("upper") != "false";
    }
    if (req.has_param("lower")) {
        options.useLower = req.get_param_value("lower") != "false";
    }
    if (req.has_param("digits")) {
        options.useDigits = req.get_param_value("digits") != "false";
    }
    if (req.has_param("symbols")) {
        options.useSymbols = req.get_param_value("symbols") != "false";
    }
    if (req.has_param("excludeAmbiguous")) {
        options.excludeAmbiguous = req.get_param_value("excludeAmbiguous") == "true";
    }
    return options;
}

int runServer(int port) {
    httplib::Server server;

    // Health check - Kubernetes ще го използва, за да провери дали подът е "жив"
    server.Get("/healthz", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    server.Get("/generate", [](const httplib::Request& req, httplib::Response& res) {
        try {
            pwdgen::GeneratorOptions options = optionsFromRequest(req);
            pwdgen::PasswordGenerator generator(options);
            std::string password = generator.generate();
            res.set_content("{\"password\":\"" + password + "\"}", "application/json");
        } catch (const std::exception& ex) {
            res.status = 400;
            res.set_content(std::string("{\"error\":\"") + ex.what() + "\"}", "application/json");
        }
    });

    std::cout << "Password generator service listening on port " << port << "\n";
    server.listen("0.0.0.0", port);
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    // Режим "serve": стартира HTTP услугата
    if (argc > 1 && std::string(argv[1]) == "serve") {
        int port = 8080;
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--port" && i + 1 < argc) {
                port = std::stoi(argv[++i]);
            }
        }
        return runServer(port);
    }

    // Стандартен CLI режим: генерира една парола и приключва
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
