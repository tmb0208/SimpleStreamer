#include "StreamServer.h"

#include <fstream>
#include <iomanip>
#include <iostream>

#include <boost/endian/conversion.hpp>

using namespace boost::asio;

StreamServer::StreamServer(io_service& io_service)
    : m_io_service(io_service)
    , m_socket(m_io_service, ip::udp::endpoint(ip::udp::v4(), 0))
{
}

void StreamServer::Run(std::ostream& stream, StreamKeyType stream_key)
{
    std::cout << "Stream started" << std::endl;
    WriteReceivedPacketsTo(stream, stream_key);
    std::cout << "Stream completed" << std::endl;
}

ip::port_type StreamServer::Port() const noexcept
{
    return m_socket.local_endpoint().port();
}

void StreamServer::WriteReceivedPacketsTo(std::ostream& stream, StreamKeyType stream_key)
{
    Packet packet;
    while (true) {
        const size_t payload_size = ReceivePacket(packet);
        if (payload_size == 0) {
            break;
        }

        if (stream_key != packet.header.stream_key) {
            ++m_discarded_packets;
            std::cerr << "Packet " << packet.header.seq_num
                      << " discarded. Unexpected stream_key "
                      << packet.header.stream_key
                      << ", expected " << stream_key << std::endl;
            continue;
        }

        if (Packet::Header::s_invalid_seq_num == packet.header.seq_num) {
            ++m_discarded_packets;
            std::cerr << "Invalid sequence number. Packet discarded." << std::endl;
            continue;
        }

        if (m_last_packet_seq_num != Packet::Header::s_invalid_seq_num) {
            if (m_last_packet_seq_num >= packet.header.seq_num) {
                std::stringstream err;
                err << "Unexpected sequence number: " << packet.header.seq_num
                    << ", last_packet_seq_num=" << m_last_packet_seq_num;
                throw std::runtime_error(err.str());
            }

            m_lost_packets += packet.header.seq_num - m_last_packet_seq_num - 1;
        }

        m_last_packet_seq_num = packet.header.seq_num;

        stream.write(packet.payload, payload_size);
        m_payload_sum += payload_size;
        Log(packet.header.stream_key);
    }

    Log(packet.header.stream_key, true);
}

size_t StreamServer::ReceivePacket(Packet& result)
{
    std::future<std::size_t> read_result = m_socket.async_receive(
        buffer(&result, sizeof(result)), boost::asio::use_future);
    constexpr auto timeout = g_gap_between_packets + std::chrono::seconds(3);
    if (read_result.wait_for(timeout) == std::future_status::timeout) {
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
    //    std::cout << "Packet received: seq_num=" << result.header.seq_num
    //              << ", stream_key=" << result.header.stream_key
    //              << ", payload_size=" << payload_size << std::endl;

    return payload_size;
}

void StreamServer::Log(StreamKeyType stream_key, bool force /*= false*/) noexcept
{
    if (m_last_logged_packet_seq_num != Packet::Header::s_invalid_seq_num
        && m_last_packet_seq_num - m_last_logged_packet_seq_num < g_packets_per_minute
        && !force) {
        return;
    }

    std::cout << "stream_key=" << stream_key
              << ", last_packet_seq_num=" << m_last_packet_seq_num
              << ", payload_sum=" << m_payload_sum
              << ", lost_packets=" << m_lost_packets
              << ", discarded_packets=" << m_discarded_packets << std::endl;
    m_last_logged_packet_seq_num = m_last_packet_seq_num;
}