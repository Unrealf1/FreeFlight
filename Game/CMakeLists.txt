cmake_minimum_required(VERSION 3.5)

project(FreeFlight-Game)

add_executable(Game "source/main.cpp")

add_executable(Example "source/example.cpp")

target_link_libraries(Game PUBLIC project_options project_warnings General)

target_link_libraries(Example PUBLIC CONAN_PKG::glew CONAN_PKG::glfw CONAN_PKG::imgui imgui_bindings)
