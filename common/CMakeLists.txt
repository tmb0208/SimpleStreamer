cmake_minimum_required(VERSION 3.2)

add_library(common STATIC
    Defs.h
    Helpers.hpp
    Offer.h Offer.cpp
    Packet.h Packet.cpp)

target_link_libraries(common ${Boost_LIBRARIES})

target_include_directories(common
    PUBLIC .)