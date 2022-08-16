#pragma once

#include "Defs.h"

#include <boost/asio.hpp>

class Offer;

class HandshakeClient {
public:
    explicit HandshakeClient(boost::asio::io_service& io_service,
        const boost::asio::ip::address& address);

    void Run(std::string_view secret_key,
        boost::asio::ip::port_type& udp_port,
        StreamKeyType& stream_key);

private:
    void SendOffer(const Offer& offer);
    boost::asio::ip::port_type WaitStreamPort();

private:
    boost::asio::io_service& m_io_service;
    boost::asio::ip::tcp::socket m_socket;
};