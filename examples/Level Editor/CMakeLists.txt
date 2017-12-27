

cmake_minimum_required(VERSION 3.0)

project(LEVEL_EDITOR)

add_executable(level_editor main.cpp LevelEditor.cpp LevelEditor.h)

target_link_libraries(level_editor xerus)
