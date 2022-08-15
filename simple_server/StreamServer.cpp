#include "StreamServer.h"

#include <fstream>
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

void StreamServer::Run(const std::string& file_path, decltype(Offer::stream_key) stream_key)
{
    std::cout << "Stream started" << std::endl;

    WriteReceivedPacketsTo(file_path, stream_key);
    std::cout << "Packets read" << std::endl;

    std::cout << "Stream completed" << std::endl;
}

ip::port_type StreamServer::Port() const noexcept
{
    return m_socket.local_endpoint().port();
}

void StreamServer::WriteReceivedPacketsTo(
    const std::string& file_path, decltype(Offer::stream_key) stream_key)
{
    std::ofstream file(file_path, std::ios::binary);
    Packet packet;
    while (m_socket.is_open()) {
        const auto payload_size = ReceivePacket(packet);
        if (payload_size <= 0) {
            continue;
        }

        if (stream_key != packet.header.stream_key) {
            std::cerr << "Packet " << packet.header.seq_num
                      << " discarded. Unexpected stream_key" << packet.header.stream_key << std::endl;
            continue;
        }

        file.write(packet.payload, payload_size);
    }
}

size_t StreamServer::ReceivePacket(Packet& result)
{
    std::future<std::size_t> read_result = m_socket.async_receive(
        buffer(&result, sizeof(result)), boost::asio::use_future);
    constexpr auto timeout = std::chrono::seconds(3);
    if (read_result.wait_for(timeout) == std::future_status::timeout) {
        m_socket.close();
        std::cout << "Socket closed" << std::endl;
        return 0;
    }

    const auto packet_size = read_result.get();
    if (packet_size <= sizeof(Packet::header)) {
        std::stringstream err;
        err << "Invalid packet: size=" << packet_size;
        throw std::runtime_error(err.str());
    }

    result.header.seq_num = boost::endian::big_to_native(result.header.seq_num);
    result.header.stream_key = boost::endian::big_to_native(result.header.stream_key);

    const size_t payload_size = packet_size - sizeof(Packet::header);
    std::cout << "Packet received: seq_num=" << result.header.seq_num
              << ", stream_key=" << result.header.stream_key
              << ", payload_size=" << payload_size << std::endl;

    return payload_size;
}