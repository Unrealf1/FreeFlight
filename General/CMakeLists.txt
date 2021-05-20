cmake_minimum_required(VERSION 3.5)

project(FreeFlight-General)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_library(General SHARED ${SOURCES})

target_link_libraries(General PUBLIC CONAN_PKG::spdlog)
target_link_libraries(General PUBLIC project_options project_warnings)

target_include_directories(General PUBLIC "./include")
