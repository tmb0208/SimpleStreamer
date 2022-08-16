#include "HandshakeClient.h"
#include "Offer.h"

#include <iostream>

#include <boost/asio/buffer.hpp>

using namespace boost::asio;

HandshakeClient::HandshakeClient(io_service& io_service,
    const boost::asio::ip::address& address)
    : m_io_service(io_service)
    , m_socket(m_io_service)
{
    m_socket.connect(ip::tcp::endpoint(address, g_handshake_port));
}

void HandshakeClient::Run(
    std::string_view secret_key,
    boost::asio::ip::port_type& udp_port,
    StreamKeyType& stream_key)
{
    std::cout << "Handshake started" << std::endl;

    const Offer offer(Offer::EType::Publisher, secret_key);
    SendOffer(offer);
    std::cout << "Offer sent" << std::endl;

    const auto port = WaitStreamPort();
    std::cout << "Port received: " << port << std::endl;

    std::cout << "Handshake Completed" << std::endl;
    udp_port = port;
    stream_key = offer.StreamKey();
}

void HandshakeClient::SendOffer(const Offer& offer)
{
    boost::system::error_code error;
    write(m_socket, buffer(offer.Serialize()), transfer_exactly(sizeof(offer)), error);
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
