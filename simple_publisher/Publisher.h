#pragma once

#include <boost/asio.hpp>

#include "Helpers.h"

class Publisher {
public:
    Publisher(std::shared_ptr<boost::asio::io_service> io_service);

    void Stream(const char* endpoint, boost::asio::ip::port_type port);

    decltype(Offer::stream_key) StreamKey() const;

private:
    void SendPackets(const char* endpoint, const boost::asio::ip::port_type port);

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::udp::socket m_stream_socket;

    decltype(Offer::stream_key) m_stream_key;
};