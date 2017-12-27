cmake_minimum_required(VERSION 3.0)

project(PICOPNG)

add_library(picopng src/picopng.cpp)

target_include_directories(picopng PUBLIC include)
