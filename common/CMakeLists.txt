cmake_minimum_required(VERSION 3.2)

add_library(common STATIC
    Helpers.h Helpers.cpp
    Offer.h Offer.cpp)

target_link_libraries(common ${Boost_LIBRARIES})

target_include_directories(common
    PUBLIC .)