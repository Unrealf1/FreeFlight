cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(FreeFlight)

option(TESTS "Build tests" OFF)
option(COVERAGE "Build with coverage" OFF)
option(EXAMPLE "Build example" OFF)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

include(cmake/Doxygen.cmake)

if (TESTS)
    set(CONAN_EXTRA_REQUIRES "gtest/1.10.0" "spdlog/1.8.5")
else()
    set(CONAN_EXTRA_REQUIRES "spdlog/1.8.5")
endif()

include(cmake/Conan.cmake)
run_conan()

if (NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

add_subdirectory("./General")

if (COVERAGE)
    include(cmake/CodeCoverage.cmake)
    target_compile_options(CommunicatorApi PUBLIC -ftest-coverage -fprofile-arcs)
    target_link_options(CommunicatorApi PUBLIC -lgcov --coverage)

    SETUP_TARGET_FOR_COVERAGE(
        coverage
        Test
        coverage
    )
endif()

if (TESTS)
    add_subdirectory("./Test")
endif(TESTS)

if (EXAMPLE)
    add_subdirectory("./Example")
endif(EXAMPLE)
