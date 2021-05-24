cmake_minimum_required(VERSION 3.5)

project(FreeFlight-Game)

add_executable(Game "source/main.cpp")

target_link_libraries(Game PUBLIC project_options project_warnings General)