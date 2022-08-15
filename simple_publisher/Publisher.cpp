#include "Publisher.h"

#include <fstream>
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

void Publisher::Stream(const char* endpoint, ip::port_type port, const std::string& file_path)
{
    m_stream_socket.connect(ip::udp::endpoint(
        ip::address::from_string(endpoint), port));
    std::cout << "Stream started" << std::endl;

    SendPackets(file_path);
    std::cout << "Packets sent" << std::endl;

    std::cout << "Stream completed" << std::endl;
}

decltype(Offer::stream_key) Publisher::StreamKey() const
{
    return m_stream_key;
}

void Publisher::SendPackets(const std::string& file_path)
{
    Packet packet { { {}, m_stream_key }, {} };
    std::ifstream file(file_path, std::ios::binary);
    while (!file.eof()) {
        file.read(packet.payload, Packet::s_max_payload_size);
        const auto bytes_read = file.gcount();
        SendPacket(packet, bytes_read);
        ++packet.header.seq_num;
    }
}

void Publisher::SendPacket(Packet packet, size_t payload_size)
{
    packet.header.seq_num = boost::endian::native_to_big(packet.header.seq_num);
    packet.header.stream_key = boost::endian::native_to_big(packet.header.stream_key);

    boost::system::error_code error;
    m_stream_socket.send(buffer(&packet, sizeof(Packet::header) + payload_size), {}, error);
    if (error) {
        std::stringstream err;
        err << "Failed to send packets: " << error.message();
        throw std::runtime_error(err.str());
    }
}
