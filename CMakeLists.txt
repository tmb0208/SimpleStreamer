cmake_minimum_required(VERSION 3.2)

project(simple_streamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.76.0 REQUIRED system)

add_subdirectory(simple_server)
add_subdirectory(simple_publisher)
add_subdirectory(protocol)