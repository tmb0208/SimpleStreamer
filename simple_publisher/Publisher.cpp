#include "Publisher.h"

#include <iostream>

#include <boost/asio/buffer.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost::asio;

namespace {
decltype(Offer::stream_key) GenerateStreamKey()
{
    return static_cast<decltype(Offer::stream_key)>(rand());
}
}

Publisher::Publisher()
    : m_handshake_socket(m_io_service)
    , m_stream_socket(m_io_service)
    , m_stream_key(GenerateStreamKey())
{
}

void Publisher::Stream(const char* secret_key)
{
    SendOffer(secret_key);
    std::cout << "Offer sent" << std::endl;

    const auto udp_port = WaitUdpPort();
    std::cout << "Port received: " << udp_port << std::endl;

    SendPackets(udp_port);
    std::cout << "Packets sent" << std::endl;

    std::cout << "Completed" << std::endl;
}

void Publisher::SendOffer(const char* secret_key)
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

    m_handshake_socket.connect(ip::tcp::endpoint(
        ip::address::from_string(s_endpoint), g_handshake_port));

    Offer offer { boost::endian::native_to_big(OfferType::Publisher), {}, m_stream_key };
    memcpy(offer.secret_key, secret_key, secret_key_size + 1 /*\0*/);
    const auto buf = buffer(&offer, sizeof(offer));
    boost::system::error_code error;
    write(m_handshake_socket, buf, transfer_exactly(sizeof(offer)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to send offer: " << error.message();
        throw std::runtime_error(err.str());
    }
}

ip::port_type Publisher::WaitUdpPort()
{
    ip::port_type result;
    boost::system::error_code error;
    read(m_handshake_socket, buffer(&result, sizeof(result)), transfer_exactly(sizeof(result)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to read UDP port: " << error.message();
        throw std::runtime_error(err.str());
    }

    return result;
}

void Publisher::SendPackets(const ip::port_type port)
{
    m_stream_socket.connect(ip::udp::endpoint(
        ip::address::from_string(s_endpoint), port));

    const decltype(Packet::payload) payload = { 't', 'e', 's', 't' };
    Packet test_packet { { boost::endian::native_to_big(5), boost::endian::native_to_big(m_stream_key) }, {} };
    memcpy(test_packet.payload, payload, sizeof(payload));
    boost::system::error_code error;
    m_stream_socket.send(buffer(&test_packet, sizeof(Packet::header) + sizeof(payload)), {}, error);
    if (error) {
        std::stringstream err;
        err << "Failed to send packets: " << error.message();
        throw std::runtime_error(err.str());
    }
}
