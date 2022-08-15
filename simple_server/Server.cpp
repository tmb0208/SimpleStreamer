#include "Server.h"

#include <iomanip>
#include <iostream>

#include <boost/endian/conversion.hpp>

#include "Helpers.h"

using namespace boost::asio;

Server::Server()
    : m_acceptor(m_io_service, ip::tcp::endpoint(ip::tcp::v4(), g_handshake_port))
    , m_handshake_socket(m_io_service)
    , m_stream_socket(m_io_service, ip::udp::endpoint(ip::udp::v4(), 0))
{
}

void Server::Run() // TODO: Make const
{
    m_acceptor.accept(m_handshake_socket);

    const Offer offer = ReadOffer();
    std::cout << "Offer received: "
              << (uint32_t)offer.offer_type << ", "
              << offer.secret_key << ", "
              << offer.stream_key << std::endl;

    if (!ValidateSecretKey(offer.secret_key)) {
        std::stringstream err;
        err << "Invalid secret key: " << offer.secret_key;
        throw std::runtime_error(err.str());
    }
    std::cout << "Secret key is valid" << std::endl;

    SendUdpPort();
    std::cout << "UDP port sent" << std::endl;

    ReadPackets();
    std::cout << "Stream read" << std::endl;

    std::cout << "Completed" << std::endl;
}

Offer Server::ReadOffer()
{
    Offer result;
    boost::system::error_code error;
    read(m_handshake_socket, buffer(&result, sizeof(result)), transfer_exactly(sizeof(result)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to read offer: " << error.message();
        throw std::runtime_error(err.str());
    }

    result.offer_type = boost::endian::big_to_native(result.offer_type);
    return result;
}

bool Server::ValidateSecretKey(const char* secret_key) const noexcept
{
    return std::strcmp(s_expected_secret_key, secret_key) == 0;
}

void Server::SendUdpPort()
{
    const ip::port_type port = m_stream_socket.local_endpoint().port();
    const auto buf = buffer(&port, sizeof(port));
    boost::system::error_code error;
    write(m_handshake_socket, buf, transfer_exactly(sizeof(port)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to send udp port: " << error.message();
        throw std::runtime_error(err.str());
    }
}

void Server::ReadPackets()
{
    Packet packet;
    boost::system::error_code error;
    m_stream_socket.receive(buffer(&packet, sizeof(packet)), {}, error);
    if (error) {
        std::stringstream err;
        err << "Failed to read stream packet: " << error.message();
        throw std::runtime_error(err.str());
    }

    packet.header.seq_num = boost::endian::big_to_native(packet.header.seq_num);
    packet.header.stream_key = boost::endian::big_to_native(packet.header.stream_key);

    std::cout << "Packet received: seq_num=" << packet.header.seq_num << ", stream_key=" << packet.header.stream_key << ", payload=";
    for (const auto byte : packet.payload)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << byte << " ";
    std::cout << std::endl;
}
