cmake_minimum_required(VERSION 3.5)

project(simple_server LANGUAGES CXX)

add_executable(simple_server
    main.cpp
    Server.h Server.cpp)

target_link_libraries(simple_server ${Boost_LIBRARIES} common)
