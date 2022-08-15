#pragma once

#include <boost/asio.hpp>

#include "Helpers.h"

class Publisher {
public:
    Publisher(std::shared_ptr<boost::asio::io_service> io_service);

    void Stream(const char* endpoint, boost::asio::ip::port_type port, const std::string& file_path);

    decltype(Offer::stream_key) StreamKey() const;

private:
    void SendPackets(const std::string& file_path);
    void SendPacket(Packet packet, size_t payload_size);

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::udp::socket m_stream_socket;

    decltype(Offer::stream_key) m_stream_key;
};