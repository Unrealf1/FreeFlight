cmake_minimum_required(VERSION 3.5)

project(FreeFlight-Test)

enable_testing()

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_executable(Test ${SOURCES})
    
target_include_directories(Test PUBLIC "./include")

target_link_libraries(Test PUBLIC project_options project_warnings General CONAN_PKG::gtest)

add_test(CommonTestsAll Test)
