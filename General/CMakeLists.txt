cmake_minimum_required(VERSION 3.5)

project(FreeFlight-General)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_library(General STATIC ${SOURCES})

target_link_libraries(General PUBLIC 
    CONAN_PKG::spdlog 
    CONAN_PKG::glm 
    CONAN_PKG::glew 
    CONAN_PKG::glfw 
    CONAN_PKG::imgui
    CONAN_PKG::jsoncpp)
    
target_link_libraries(General PUBLIC soil imgui_bindings)
target_link_libraries(General PUBLIC project_options project_warnings)

target_include_directories(General PUBLIC "./include")
