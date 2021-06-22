cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE SOURCES . ./*.cpp)

add_library(imgui_bindings STATIC ${SOURCES})

target_link_libraries(imgui_bindings PUBLIC CONAN_PKG::imgui CONAN_PKG::glew CONAN_PKG::glfw)
target_include_directories(imgui_bindings PUBLIC ".")
