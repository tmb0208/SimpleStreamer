#pragma once

#include <memory>

#include <boost/asio.hpp>

class StreamServer {
public:
    StreamServer(std::shared_ptr<boost::asio::io_service> io_service);

    void Run();

    boost::asio::ip::port_type Port() const noexcept;

private:
    void ReadPackets();

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::udp::socket m_stream_socket;
};
