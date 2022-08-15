#pragma once

#include <boost/asio.hpp>

struct Offer;

class HandshakeServer {

public:
    HandshakeServer(std::shared_ptr<boost::asio::io_service> io_service);

    void Run(const boost::asio::ip::port_type stream_port);

private:
    Offer ReadOffer();
    bool ValidateSecretKey(const char* secret_key) const noexcept;
    void SendStreamPort(const boost::asio::ip::port_type port);

private:
    static constexpr char s_expected_secret_key[] = "secret_key";

    std::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
};