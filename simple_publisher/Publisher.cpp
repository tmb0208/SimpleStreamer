#include "Publisher.h"

#include <fstream>
#include <iostream>

#include <boost/endian/conversion.hpp>

using namespace boost::asio;

namespace {
StreamKeyType GenerateStreamKey()
{
    return static_cast<StreamKeyType>(rand());
}
}

Publisher::Publisher(std::shared_ptr<boost::asio::io_service> io_service)
    : m_io_service(io_service)
    , m_socket(*m_io_service)
    , m_stream_key(GenerateStreamKey())
{
}

void Publisher::Stream(const char* endpoint, ip::port_type port, const std::string& file_path)
{
    m_socket.connect(ip::udp::endpoint(
        ip::address::from_string(endpoint), port));
    std::cout << "Stream started" << std::endl;
    SendPackets(file_path);
    std::cout << "Stream completed" << std::endl;
}

StreamKeyType Publisher::StreamKey() const
{
    return m_stream_key;
}

void Publisher::SendPackets(const std::string& file_path)
{
    Packet packet;
    packet.header.stream_key = boost::endian::native_to_big(m_stream_key);
    std::ifstream file(file_path, std::ios::binary);
    while (!file.eof()) {
        file.read(packet.payload, Packet::s_max_payload_size);
        packet.header.seq_num = boost::endian::native_to_big(m_last_packet_seq_num);
        const auto bytes_read = file.gcount();
        SendPacket(packet, bytes_read);
        Log();
        ++m_last_packet_seq_num;
    }
}

void Publisher::SendPacket(const Packet& packet, size_t payload_size)
{
    boost::system::error_code error;
    m_socket.send(buffer(&packet, sizeof(Packet::header) + payload_size), {}, error);
    if (error) {
        std::stringstream err;
        err << "Failed to send packets: " << error.message();
        throw std::runtime_error(err.str());
    }

    m_payload_sum += payload_size;
}

void Publisher::Log() const noexcept
{
    if (m_last_packet_seq_num % s_packets_log_frequency != 0) {
        return;
    }

    std::cout << "stream_key=" << m_stream_key
              << ", last_packet_seq_num=" << m_last_packet_seq_num
              << ", payload_sum=" << m_payload_sum << std::endl;
}
