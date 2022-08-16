#pragma once

#include "Defs.h"

#include <boost/asio.hpp>

class Offer;

class HandshakeServer {

public:
    explicit HandshakeServer(boost::asio::io_service& io_service);

    StreamKeyType Run(boost::asio::ip::port_type stream_port);

private:
    Offer ReadOffer();
    void SendStreamPort(boost::asio::ip::port_type port);

private:
    static constexpr std::string_view s_expected_secret_key = "secret_key";

    boost::asio::io_service& m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
};