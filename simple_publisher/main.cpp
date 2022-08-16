#include "HandshakeClient.h"
#include "Publisher.h"

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: simple_publisher [file]  Stream specified file" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1], std::ios::binary);

    boost::asio::io_service io_service;
    Publisher publisher(io_service);
    HandshakeClient handshake_client(io_service);
    constexpr std::string_view endpoint = "127.0.0.1";
    char* secret_key = std::getenv("PUBLISHER_SECRET");
    if (secret_key == nullptr) {
        throw std::runtime_error("PUBLISHER_SECRET environment variable not initialized");
    }

    const auto stream_port = handshake_client.Run("127.0.0.1", secret_key, publisher.StreamKey());
    publisher.Stream(endpoint, stream_port, file);
    return 0;
}