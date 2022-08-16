#include "HandshakeServer.h"
#include "StreamServer.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: simple_server [file]  Receives udp stream and save to file" << std::endl;
        return 1;
    }

    boost::asio::io_service io_service;
    boost::asio::io_service::work work(io_service);
    std::thread thread([&io_service]() { io_service.run(); });

    StreamServer stream_server(io_service);
    HandshakeServer handshake_server(io_service);
    const auto stream_key = handshake_server.Run(stream_server.Port());
    std::ofstream file(argv[1], std::ios::binary);
    stream_server.Run(stream_key, file);
    io_service.stop();
    thread.join();
    return 0;
}
