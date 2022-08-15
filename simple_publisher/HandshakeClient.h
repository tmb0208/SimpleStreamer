#pragma once

#include <boost/asio.hpp>

#include "Helpers.h"

class HandshakeClient {
public:
    HandshakeClient(std::shared_ptr<boost::asio::io_service> io_service);

    boost::asio::ip::port_type Run(
        const char* endpoint, const char* secret_key, StreamKeyType stream_key);

private:
    void SendOffer(
        const char* endpoint, const char* secret_key, StreamKeyType stream_key);
    boost::asio::ip::port_type WaitStreamPort();

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::tcp::socket m_socket;
};