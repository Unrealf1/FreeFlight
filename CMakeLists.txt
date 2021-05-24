cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(FreeFlight)

option(TESTS "Build tests" OFF)
option(COVERAGE "Build with coverage" OFF)

add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

include(cmake/Doxygen.cmake)

# later can add assimp/5.0.1 for model import
set(CONAN_BASE_DEPENDENCIES "spdlog/1.8.5"
    "glm/0.9.9.8"
    "glew/2.2.0" 
    "glfw/3.3.4" 
    "imgui/1.82" 
    "jsoncpp/1.9.4" )
set(CONAN_TEST_DEPENDENCIES "gtest/1.10.0")

if (TESTS)
    set(CONAN_EXTRA_REQUIRES ${CONAN_BASE_DEPENDENCIES} ${CONAN_TEST_DEPENDENCIES})
else()
    set(CONAN_EXTRA_REQUIRES ${CONAN_BASE_DEPENDENCIES})
endif()

include(cmake/Conan.cmake)
run_conan()


if (NOT APPLE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

add_subdirectory("./External")

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

add_subdirectory("./Game")
