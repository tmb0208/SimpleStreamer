#include <iostream>

#include "HandshakeServer.h"
#include "StreamServer.h"

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage: simple_server [file]  Receives udp stream and save to file" << std::endl;
        return 1;
    }

    auto io_service = std::make_shared<boost::asio::io_service>();
    boost::asio::io_service::work work(*io_service);
    std::thread thread([io_service]() { io_service->run(); });

    StreamServer stream_server(io_service);
    HandshakeServer handshake_server(io_service);
    const auto stream_key = handshake_server.Run(stream_server.Port());
    const auto file_path = argv[1];
    stream_server.Run(file_path, stream_key);

    io_service->stop();
    thread.join();
    return 0;
}
