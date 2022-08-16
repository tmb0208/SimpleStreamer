#include "HandshakeServer.h"

#include <iostream>

#include <boost/endian/conversion.hpp>

#include "Helpers.h"

using namespace boost::asio;

HandshakeServer::HandshakeServer(io_service& io_service)
    : m_io_service(io_service)
    , m_acceptor(m_io_service, ip::tcp::endpoint(ip::tcp::v4(), g_handshake_port))
    , m_socket(m_io_service)
{
    m_acceptor.accept(m_socket);
}

StreamKeyType HandshakeServer::Run(ip::port_type stream_port)
{
    std::cout << "Handshake started" << std::endl;

    auto offer = ReadOffer();
    std::cout << "Offer received: "
              << (uint32_t)offer.offer_type << ", "
              << offer.secret_key << ", "
              << offer.stream_key << std::endl;

    if (!ValidateSecretKey(offer)) {
        std::stringstream err;
        err << "Invalid secret key: " << offer.secret_key;
        throw std::runtime_error(err.str());
    }
    std::cout << "Secret key is valid" << std::endl;

    SendStreamPort(stream_port);
    std::cout << "Stream port sent" << std::endl;

    std::cout << "Handshake completed" << std::endl;
    return offer.stream_key;
}

Offer HandshakeServer::ReadOffer()
{
    Offer result;
    boost::system::error_code error;
    read(m_socket, buffer(&result, sizeof(result)), transfer_exactly(sizeof(result)), error);
    if (error) {
        detail::throw_error(error, "Failed to read offer");
    }

    result.offer_type = boost::endian::big_to_native(result.offer_type);
    result.stream_key = boost::endian::big_to_native(result.stream_key);
    return result;
}

bool HandshakeServer::ValidateSecretKey(const Offer& offer) const noexcept // TODO: Move to Offer class
{
    return s_expected_secret_key == offer.secret_key && offer.offer_type == Offer::Type::Publisher;
}

void HandshakeServer::SendStreamPort(ip::port_type port)
{
    boost::system::error_code error;
    write(m_socket, buffer(&port, sizeof(port)), transfer_exactly(sizeof(port)), error);
    if (error) {
        detail::throw_error(error, "Failed to send udp port");
    }
}
