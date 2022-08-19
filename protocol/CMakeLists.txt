cmake_minimum_required(VERSION 3.2)

add_library(protocol STATIC
    Defs.h
    Helpers.h Helpers.hpp
    Offer.h Offer.cpp
    Packet.h Packet.cpp)

target_link_libraries(protocol ${Boost_LIBRARIES})

target_include_directories(protocol
    PUBLIC .)