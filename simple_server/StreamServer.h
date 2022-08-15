#pragma once

#include <memory>

#include <boost/asio.hpp>

#include "Helpers.h"

class StreamServer {
public:
    StreamServer(std::shared_ptr<boost::asio::io_service> io_service);

    void Run(const std::string& file_path, decltype(Offer::stream_key) stream_key);

    boost::asio::ip::port_type Port() const noexcept;

private:
    void WriteReceivedPacketsTo(const std::string& file_path, decltype(Offer::stream_key) stream_key);
    size_t ReceivePacket(Packet& result);

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::udp::socket m_socket;
};
