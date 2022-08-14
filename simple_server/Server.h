#pragma once

#include <boost/asio.hpp>

struct Offer;

class Server {

public:
    Server();

    void Run();

private:
    Offer ReadOffer();
    bool ValidateSecretKey(const char* secret_key) const noexcept;
    void SendUdpPort();

private:
    static constexpr char expected_secret_key[] = "secret_key";

    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_socket;
};