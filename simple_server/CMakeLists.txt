cmake_minimum_required(VERSION 3.2)

add_executable(simple_server
    main.cpp
    HandshakeServer.h HandshakeServer.cpp
    StreamServer.h StreamServer.cpp)

target_link_libraries(simple_server ${Boost_LIBRARIES} protocol)
