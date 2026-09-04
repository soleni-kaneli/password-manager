cmake_minimum_required(VERSION 3.14)
project(password_generator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Библиотека с логиката (за да я ползваме и в main, и в тестовете)
add_library(generator_lib src/generator.cpp)
target_include_directories(generator_lib PUBLIC include)

# Изпълнимият файл на приложението
add_executable(password_generator src/main.cpp)
target_link_libraries(password_generator PRIVATE generator_lib)

# Тестове - включват се само ако е зададено BUILD_TESTS
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(unit_tests tests/test_generator.cpp)
    target_link_libraries(unit_tests PRIVATE generator_lib GTest::gtest GTest::gtest_main)
    add_test(NAME unit_tests COMMAND unit_tests)
endif()
