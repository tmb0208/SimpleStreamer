#include "HandshakeClient.h"

#include <iostream>

#include <boost/asio/buffer.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost::asio;

HandshakeClient::HandshakeClient(std::shared_ptr<boost::asio::io_service> io_service)
    : m_io_service(io_service)
    , m_socket(*m_io_service)
{
}

ip::port_type HandshakeClient::Run(
    const char* endpoint, const char* secret_key, decltype(Offer::stream_key) stream_key)
{
    std::cout << "Handshake started" << std::endl;

    SendOffer(endpoint, secret_key, stream_key);
    std::cout << "Offer sent" << std::endl;

    const auto port = WaitStreamPort();
    std::cout << "Port received: " << port << std::endl;

    std::cout << "Handshake Completed" << std::endl;
    return port;
}

void HandshakeClient::SendOffer(
    const char* endpoint, const char* secret_key, decltype(Offer::stream_key) stream_key)
{
    if (secret_key == nullptr) {
        throw std::runtime_error("PUBLISHER_SECRET environment variable not initialized");
    }

    const size_t secret_key_size = strlen(secret_key);
    if (secret_key_size > sizeof(Offer::secret_key)) {
        std::stringstream err;
        err << "Secret key is longer than "
            << std::to_string(sizeof(Offer::secret_key))
            << ": " << secret_key
            << '(' << std::to_string(secret_key_size) << ')';
        throw std::runtime_error(err.str());
    }

    m_socket.connect(ip::tcp::endpoint(
        ip::address::from_string(endpoint), g_handshake_port));

    Offer offer { boost::endian::native_to_big(OfferType::Publisher), {}, stream_key };
    memcpy(offer.secret_key, secret_key, secret_key_size + 1 /*\0*/);
    const auto buf = buffer(&offer, sizeof(offer));
    boost::system::error_code error;
    write(m_socket, buf, transfer_exactly(sizeof(offer)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to send offer: " << error.message();
        throw std::runtime_error(err.str());
    }
}

ip::port_type HandshakeClient::WaitStreamPort()
{
    ip::port_type result;
    boost::system::error_code error;
    read(m_socket, buffer(&result, sizeof(result)), transfer_exactly(sizeof(result)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to read UDP port: " << error.message();
        throw std::runtime_error(err.str());
    }

    return result;
}
