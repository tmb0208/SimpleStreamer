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
    void ReadPackets();

private:
    static constexpr char s_expected_secret_key[] = "secret_key";

    boost::asio::io_service m_io_service;
    boost::asio::ip::tcp::acceptor m_acceptor;
    boost::asio::ip::tcp::socket m_handshake_socket;

    boost::asio::ip::udp::socket m_stream_socket;
};