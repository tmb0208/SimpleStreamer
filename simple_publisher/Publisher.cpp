#include "Publisher.h"

#include <iostream>

#include <boost/endian/conversion.hpp>

using namespace boost::asio;

namespace {
decltype(Offer::stream_key) GenerateStreamKey()
{
    return static_cast<decltype(Offer::stream_key)>(rand());
}
}

Publisher::Publisher(std::shared_ptr<boost::asio::io_service> io_service)
    : m_io_service(io_service)
    , m_stream_socket(*m_io_service)
    , m_stream_key(GenerateStreamKey())
{
}

void Publisher::Stream(const char* endpoint, ip::port_type port)
{
    std::cout << "Stream started" << std::endl;

    SendPackets(endpoint, port);
    std::cout << "Packets sent" << std::endl;

    std::cout << "Stream completed" << std::endl;
}

decltype(Offer::stream_key) Publisher::StreamKey() const
{
    return m_stream_key;
}

void Publisher::SendPackets(const char* endpoint, const ip::port_type port)
{
    m_stream_socket.connect(ip::udp::endpoint(
        ip::address::from_string(endpoint), port));

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
