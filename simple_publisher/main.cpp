#include "HandshakeClient.h"
#include "Defs.h"
#include "Publisher.h"

#include <fstream>
#include <iostream>

#include <boost/asio.hpp>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: simple_publisher [file]  Stream specified file" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1], std::ios::binary);

    boost::asio::io_service io_service;

    const auto address = boost::asio::ip::make_address("127.0.0.1");
    const char* secret_key = std::getenv("PUBLISHER_SECRET");
    if (secret_key == nullptr) {
        throw std::runtime_error("PUBLISHER_SECRET environment variable not initialized");
    }

    HandshakeClient handshake_client(io_service, address);
    StreamKeyType stream_key;
    boost::asio::ip::port_type port = 0;
    handshake_client.Run(secret_key, port, stream_key);

    Publisher publisher(io_service, address, port, stream_key);
    publisher.Stream(file);
    return 0;
}