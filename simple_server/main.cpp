#include "HandshakeServer.h"
#include "StreamServer.h"

int main()
{
    auto io_service = std::make_shared<boost::asio::io_service>();
    StreamServer stream_server(io_service);
    HandshakeServer handshake_server(io_service);
    handshake_server.Run(stream_server.Port());
    stream_server.Run();
    return 0;
}
