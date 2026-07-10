cmake_minimum_required(VERSION 3.20)

project(
    Redix
    VERSION 0.1.0
    DESCRIPTION "Redis inspired key value store in C++20"
    LANGUAGES CXX
)

# ---------------------------------------------------------
# C++ Standard
# ---------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------
# Build Type
# ---------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---------------------------------------------------------
# Warnings
# ---------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
    )
endif()

# ---------------------------------------------------------
# Core Library
# ---------------------------------------------------------

add_library(
    redix_core
    src/parser/parser.cpp
)

target_include_directories(
    redix_core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/src
)

# ---------------------------------------------------------
# Main Executable
# ---------------------------------------------------------

add_executable(
    redix
    src/main.cpp
)

target_link_libraries(
    redix
    PRIVATE
    redix_core
)

# ---------------------------------------------------------
# Test Executable
# ---------------------------------------------------------

add_executable(
    redix_tests
    tests/parser_tests.cpp
)

target_link_libraries(
    redix_tests
    PRIVATE
    redix_core
)

# ---------------------------------------------------------
# Sanitizers
# ---------------------------------------------------------

option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")

    foreach(target redix redix_tests)

        target_compile_options(
            ${target}
            PRIVATE
            -fsanitize=address
            -fsanitize=undefined
        )

        target_link_options(
            ${target}
            PRIVATE
            -fsanitize=address
            -fsanitize=undefined
        )

    endforeach()

endif()

# ---------------------------------------------------------
# Output Directory
# ---------------------------------------------------------

set_target_properties(
    redix
    redix_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/bin
)