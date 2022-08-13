cmake_minimum_required(VERSION 3.5)

project(simple_streamer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_ROOT "$ENV{HOME}/boost/boost_1_76_0")
set(Boost_INCLUDE_DIR  "$ENV{HOME}/boost/boost_1_76_0")
set(Boost_LIBRARY_DIR "$ENV{HOME}/boost/boost_1_76_0/libs")

find_package(Boost 1.76.0 REQUIRED system)

set(subdirs
    simple_server
    simple_publisher)

foreach(subdir ${subdirs})
    add_subdirectory(${subdir})
endforeach()