#pragma once

#include <boost/asio.hpp>

#include "Helpers.h"

class Publisher {
public:
    Publisher();

    void Stream(const char* secret_key);

private:
    void SendOffer(const char* secret_key);
    boost::asio::ip::port_type WaitUdpPort();
    void SendPackets(const boost::asio::ip::port_type port);

private:
    static constexpr auto s_endpoint = "127.0.0.1";

    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::socket m_handshake_socket;
    boost::asio::ip::udp::socket m_stream_socket;

    decltype(Offer::stream_key) m_stream_key;
};