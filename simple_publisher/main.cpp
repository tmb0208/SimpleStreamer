#include "HandshakeClient.h"
#include "Publisher.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: simple_publisher [file]  Stream specified file" << std::endl;
        return 1;
    }
    const auto file_path = argv[1];

    auto io_service = std::make_shared<boost::asio::io_service>();
    Publisher publisher(io_service);
    HandshakeClient handshake_client(io_service);
    constexpr auto endpoint = "127.0.0.1";
    char* secret_key = std::getenv("PUBLISHER_SECRET");
    const auto stream_port = handshake_client.Run(endpoint, secret_key, publisher.StreamKey());
    publisher.Stream(endpoint, stream_port, file_path);
    return 0;
}