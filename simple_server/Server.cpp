#include "Server.h"

#include <iostream>

#include <boost/endian/conversion.hpp>

#include "Helpers.h"

using namespace boost::asio;

Server::Server()
    : m_acceptor(m_io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT))
    , m_socket(m_io_service)
{
}

void Server::Run() // TODO: Make const
{
    m_acceptor.accept(m_socket);

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

    std::cout << "Completed" << std::endl;
}

Offer Server::ReadOffer()
{
    Offer result;
    streambuf buf(sizeof(result));
    boost::system::error_code error;
    read(m_socket, buf, transfer_exactly(sizeof(result)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to read offer: " << error.message();
        throw std::runtime_error(err.str());
    }

    memcpy(&result, buffer_cast<const char*>(buf.data()), buf.size());
    result.offer_type = boost::endian::big_to_native(result.offer_type);
    return result;
}

bool Server::ValidateSecretKey(const char* secret_key) const noexcept
{
    return std::strcmp(expected_secret_key, secret_key) == 0;
}

void Server::SendUdpPort()
{
    constexpr udp_port_type dummy_udp_port = 5000; // TODO
    const auto buf = buffer(&dummy_udp_port, sizeof(dummy_udp_port));
    boost::system::error_code error;
    write(m_socket, buf, transfer_exactly(sizeof(dummy_udp_port)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to send udp port: " << error.message();
        throw std::runtime_error(err.str());
    }
}
