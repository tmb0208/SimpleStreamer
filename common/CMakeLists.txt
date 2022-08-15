cmake_minimum_required(VERSION 3.5)

set(inc
    Helpers.cpp)

set(src
    Helpers.h)

add_library(common STATIC
    ${src}
    ${inc})

target_include_directories(common
    PUBLIC .)