cmake_minimum_required(VERSION 3.5)

project(simple_publisher LANGUAGES CXX)

add_executable(simple_publisher main.cpp)

target_link_libraries(simple_publisher ${Boost_LIBRARIES})
