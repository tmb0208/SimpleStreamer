#include "HandshakeClient.h"

#include <iostream>

#include <boost/asio/buffer.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost::asio;

HandshakeClient::HandshakeClient(io_service& io_service)
    : m_io_service(io_service)
    , m_socket(m_io_service)
{
}

ip::port_type HandshakeClient::Run(
    std::string_view endpoint, std::string_view secret_key, StreamKeyType stream_key)
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
    std::string_view endpoint, std::string_view secret_key, StreamKeyType stream_key)
{
    if (secret_key.size() > Offer::s_secret_key_size) {
        std::stringstream err;
        err << "Secret key is longer than "
            << Offer::s_secret_key_size
            << ": " << secret_key
            << '(' << secret_key.size() << ')';
        throw std::runtime_error(err.str());
    }

    m_socket.connect(ip::tcp::endpoint(ip::make_address(endpoint), g_handshake_port));

    Offer offer { boost::endian::native_to_big(Offer::Type::Publisher), {}, boost::endian::native_to_big(stream_key) };
    memcpy(offer.secret_key, secret_key.data(), secret_key.size() + 1 /*\0*/);
    boost::system::error_code error;
    write(m_socket, buffer(&offer, sizeof(offer)), transfer_exactly(sizeof(offer)), error);
    if (error) {
        detail::throw_error(error, "Failed to send offer");
    }
}

ip::port_type HandshakeClient::WaitStreamPort()
{
    ip::port_type result;
    boost::system::error_code error;
    read(m_socket, buffer(&result, sizeof(result)), transfer_exactly(sizeof(result)), error);
    if (error) {
        detail::throw_error(error, "Failed to read UDP port");
    }

    return result;
}
