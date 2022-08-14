cmake_minimum_required(VERSION 3.5)

set(src
    Helpers.h)

set(inc
    Helpers.cpp)

add_library(common STATIC
    ${src}
    ${inc})

target_include_directories(common
    PUBLIC .)