#include "StreamServer.h"

#include <iomanip>
#include <iostream>

#include <boost/endian/conversion.hpp>

#include "Helpers.h"

using namespace boost::asio;

StreamServer::StreamServer(std::shared_ptr<boost::asio::io_service> io_service)
    : m_io_service(io_service)
    , m_socket(*m_io_service, ip::udp::endpoint(ip::udp::v4(), 0))
{
}

void StreamServer::Run()
{
    std::cout << "Stream started" << std::endl;

    ReadPackets();
    std::cout << "Packets read" << std::endl;

    std::cout << "Stream completed" << std::endl;
}

ip::port_type StreamServer::Port() const noexcept
{
    return m_socket.local_endpoint().port();
}

void StreamServer::ReadPackets()
{
    Packet packet;
    boost::system::error_code error;
    const auto packet_size = m_socket.receive(buffer(&packet, sizeof(packet)), {}, error);
    if (error) {
        std::stringstream err;
        err << "Failed to read stream packet: " << error.message();
        throw std::runtime_error(err.str());
    }

    packet.header.seq_num = boost::endian::big_to_native(packet.header.seq_num);
    packet.header.stream_key = boost::endian::big_to_native(packet.header.stream_key);

    const auto payload_size = packet_size - sizeof(Packet::header);
    std::cout << "Packet received: seq_num=" << packet.header.seq_num
              << ", stream_key=" << packet.header.stream_key
              << ", payload_size=" << payload_size
              << ", payload=";
    for (size_t i = 0; i < payload_size; ++i)
        std::cout << std::hex << std::setfill('0') << std::setw(2) << packet.payload[i] << " ";
    std::cout << std::endl;
}