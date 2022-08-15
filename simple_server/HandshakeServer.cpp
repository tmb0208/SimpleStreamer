#include "HandshakeServer.h"

#include <iostream>

#include <boost/endian/conversion.hpp>

#include "Helpers.h"

using namespace boost::asio;

HandshakeServer::HandshakeServer(std::shared_ptr<io_service> io_service)
    : m_io_service(io_service)
    , m_acceptor(*m_io_service, ip::tcp::endpoint(ip::tcp::v4(), g_handshake_port))
    , m_handshake_socket(*m_io_service)
{
}

void HandshakeServer::Run(const ip::port_type stream_port) // TODO: Make const
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

    SendStreamPort(stream_port);
    std::cout << "Stream port sent" << std::endl;

    std::cout << "Handshake completed" << std::endl;
}

Offer HandshakeServer::ReadOffer()
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

bool HandshakeServer::ValidateSecretKey(const char* secret_key) const noexcept
{
    return std::strcmp(s_expected_secret_key, secret_key) == 0;
}

void HandshakeServer::SendStreamPort(const ip::port_type port)
{
    const auto buf = buffer(&port, sizeof(port));
    boost::system::error_code error;
    write(m_handshake_socket, buf, transfer_exactly(sizeof(port)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to send udp port: " << error.message();
        throw std::runtime_error(err.str());
    }
}
