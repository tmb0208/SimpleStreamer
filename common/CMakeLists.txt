cmake_minimum_required(VERSION 3.2)

add_library(common STATIC
    Helpers.h Helpers.cpp)

target_include_directories(common
    PUBLIC .)