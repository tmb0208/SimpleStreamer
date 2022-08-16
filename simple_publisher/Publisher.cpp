#include "Publisher.h"

#include <iostream>

#include <boost/endian/conversion.hpp>

using namespace boost::asio;

namespace {
StreamKeyType GenerateStreamKey()
{
    return static_cast<StreamKeyType>(rand());
}
}

Publisher::Publisher(boost::asio::io_service& io_service)
    : m_io_service(io_service)
    , m_socket(m_io_service)
    , m_stream_key(GenerateStreamKey())
{
}

void Publisher::Stream(std::string_view endpoint, ip::port_type port, std::istream& stream)
{
    m_socket.connect(ip::udp::endpoint(
        ip::make_address(endpoint), port));
    std::cout << "Stream started" << std::endl;
    SendPackets(stream);
    std::cout << "Stream completed" << std::endl;
}

StreamKeyType Publisher::StreamKey() const noexcept
{
    return m_stream_key;
}

void Publisher::SendPackets(std::istream& stream)
{
    Packet packet;
    packet.header.stream_key = boost::endian::native_to_big(m_stream_key);
    while (stream.good()) {
        stream.read(packet.payload, Packet::s_max_payload_size);
        const auto bytes_read = stream.gcount();
        packet.header.seq_num = boost::endian::native_to_big(m_last_packet_seq_num);
        SendPacket(packet, bytes_read);
        Log();
        ++m_last_packet_seq_num;
        std::this_thread::sleep_for(g_gap_between_packets);
    }

    Log(true);
}

void Publisher::SendPacket(const Packet& packet, size_t payload_size)
{
    boost::system::error_code error;
    m_socket.send(buffer(&packet, sizeof(Packet::header) + payload_size), {}, error);
    if (error) {
        detail::throw_error(error, "Failed to send packets");
    }

    m_payload_sum += payload_size;
}

void Publisher::Log(bool force /*= false*/) const noexcept
{
    if (m_last_packet_seq_num % g_packets_per_minute != 0 && !force) {
        return;
    }

    std::cout << "stream_key=" << m_stream_key
              << ", last_packet_seq_num=" << m_last_packet_seq_num
              << ", payload_sum=" << m_payload_sum << std::endl;
}
