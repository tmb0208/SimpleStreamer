#pragma once

#include <boost/asio.hpp>

#include "Helpers.h"

class Publisher {
public:
    Publisher();

    void Stream(const char* secret_key);

private:
    void SendOffer(const char* secret_key);
    udp_port_type WaitUdpPort();

private:
    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::socket m_socket;
};