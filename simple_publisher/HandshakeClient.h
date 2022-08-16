#pragma once

#include "Helpers.h"

#include <boost/asio.hpp>

class HandshakeClient {
public:
    explicit HandshakeClient(boost::asio::io_service& io_service);

    boost::asio::ip::port_type Run(
        std::string_view endpoint, std::string_view secret_key, StreamKeyType stream_key);

private:
    void SendOffer(
        std::string_view endpoint, std::string_view secret_key, StreamKeyType stream_key);
    boost::asio::ip::port_type WaitStreamPort();

private:
    boost::asio::io_service& m_io_service;
    boost::asio::ip::tcp::socket m_socket;
};