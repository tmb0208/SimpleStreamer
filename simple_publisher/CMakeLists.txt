cmake_minimum_required(VERSION 3.2)

add_executable(simple_publisher
    main.cpp
    HandshakeClient.h HandshakeClient.cpp
    Publisher.h Publisher.cpp)

target_link_libraries(simple_publisher ${Boost_LIBRARIES} protocol)
