#include "HandshakeServer.h"

#include <iostream>

#include <boost/endian/conversion.hpp>

#include "Defs.h"
#include "Offer.h"

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
              << static_cast<uint32_t>(offer.Type()) << ", "
              << offer.SecretKey() << ", "
              << offer.StreamKey() << std::endl;

    if (!offer.Validate(s_expected_secret_key, Offer::EType::Publisher)) {
        throw std::runtime_error("Invalid secret key");
    }
    std::cout << "Secret key is valid" << std::endl;

    SendStreamPort(stream_port);
    std::cout << "Stream port sent" << std::endl;

    std::cout << "Handshake completed" << std::endl;
    return offer.StreamKey();
}

Offer HandshakeServer::ReadOffer()
{
    std::vector<std::byte> data(Offer::s_serialized_size);
    boost::system::error_code error;
    read(m_socket, buffer(data), transfer_exactly(Offer::s_serialized_size), error);
    if (error) {
        detail::throw_error(error, "Failed to read offer");
    }

    return Offer::Deserialize(data);
}

void HandshakeServer::SendStreamPort(ip::port_type port)
{
    boost::system::error_code error;
    write(m_socket, buffer(&port, sizeof(port)), transfer_exactly(sizeof(port)), error);
    if (error) {
        detail::throw_error(error, "Failed to send udp port");
    }
}
