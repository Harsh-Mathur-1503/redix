cmake_minimum_required(VERSION 3.20)

project(
    Redix
    VERSION 0.1.0
    DESCRIPTION "Redis inspired key value store in C++20"
    LANGUAGES CXX
)

#---------------------------------------------------------
# C++ Standard
#---------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#---------------------------------------------------------
# Build Type
#---------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

#---------------------------------------------------------
# Warnings
#---------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "clang|GNU")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
    )
endif()
#---------------------------------------------------------
# Executable
#---------------------------------------------------------
add_executable(
    redix_tests
    tests/parser_tests.cpp
)

target_link_libraries(
    redix_tests
    PRIVATE
    redix_core
)

#---------------------------------------------------------
# Include Directories
#---------------------------------------------------------
target_include_directories(
    redix
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)


option(ENABLE_SANITIZERS "Enable sanitizers" ON)

if(ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(
        redix
        PRIVATE
        -fsanitize=address
        -fsanitize=undefined
    )

    target_link_options(
        redix
        PRIVATE
        -fsanitize=address
        -fsanitize=undefined
    )
endif()

#---------------------------------------------------------
# Output Directory
#---------------------------------------------------------
set_target_properties(
    redix
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_SOURCE_DIR}/bin
)