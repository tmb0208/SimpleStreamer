#include "HandshakeClient.h"
#include "Publisher.h"

int main()
{
    constexpr auto endpoint = "127.0.0.1";
    char* secret_key = std::getenv("PUBLISHER_SECRET");
    auto io_service = std::make_shared<boost::asio::io_service>();
    Publisher publisher(io_service);
    HandshakeClient handshake_client(io_service);
    const auto stream_port = handshake_client.Run(endpoint, secret_key, publisher.StreamKey());
    publisher.Stream(endpoint, stream_port);
    return 0;
}