#include "StreamServer.h"

#include <fstream>
#include <iomanip>
#include <iostream>

#include <boost/endian/conversion.hpp>

using namespace boost::asio;

struct TimeoutException { };

StreamServer::StreamServer(io_service& io_service)
    : m_io_service(io_service)
    , m_socket(m_io_service, ip::udp::endpoint(ip::udp::v4(), 0))
{
}

void StreamServer::Run(StreamKeyType stream_key, std::ostream& stream)
{
    m_stream_key = stream_key;
    std::cout << "Stream started" << std::endl;
    WriteReceivedPacketsTo(stream);
    std::cout << "Stream completed" << std::endl;
}

ip::port_type StreamServer::Port() const noexcept
{
    return m_socket.local_endpoint().port();
}

void StreamServer::WriteReceivedPacketsTo(std::ostream& stream)
{
    while (true) {
        try {
            const Packet packet = ReceivePacket();

            if (m_stream_key != packet.StreamKey()) {
                ++m_discarded_packets;
                std::cerr << "Packet " << packet.SeqNum()
                          << " discarded. Unexpected stream_key "
                          << packet.StreamKey()
                          << ", expected " << m_stream_key << std::endl;
                continue;
            }

            if (Packet::s_invalid_seq_num == packet.SeqNum()) {
                ++m_discarded_packets;
                std::cerr << "Invalid sequence number. Packet discarded." << std::endl;
                continue;
            }

            if (m_last_packet_seq_num != Packet::s_invalid_seq_num) {
                if (m_last_packet_seq_num >= packet.SeqNum()) {
                    std::stringstream err;
                    err << "Unexpected sequence number: " << packet.SeqNum()
                        << ", last_packet_seq_num=" << m_last_packet_seq_num;
                    throw std::runtime_error(err.str());
                }

                m_lost_packets += packet.SeqNum() - m_last_packet_seq_num - 1;
            }

            m_last_packet_seq_num = packet.SeqNum();

            const auto& payload = packet.Payload();
            stream.write(payload.data(), payload.size());
            m_payload_sum += payload.size();
            Log(packet.StreamKey());
        } catch (TimeoutException) {
            break;
        }
    }

    Log(true);
}

Packet StreamServer::ReceivePacket()
{
    std::vector<std::byte> buf;
    buf.resize(Packet::s_max_packet_size); // TODO: resize?
    std::future<std::size_t> read_result = m_socket.async_receive(
        buffer(buf), boost::asio::use_future);
    constexpr auto timeout = g_gap_between_packets + std::chrono::seconds(3);
    if (read_result.wait_for(timeout) == std::future_status::timeout) {
        throw TimeoutException();
    }

    const size_t bytes_read = read_result.get();
    buf.resize(bytes_read);
    return Packet::Deserialize(buf);
}

void StreamServer::Log(bool force /*= false*/) noexcept
{
    if (m_last_logged_packet_seq_num != Packet::s_invalid_seq_num
        && m_last_packet_seq_num - m_last_logged_packet_seq_num < g_packets_per_minute
        && !force) {
        return;
    }

    std::cout << "stream_key=" << m_stream_key
              << ", last_packet_seq_num=" << m_last_packet_seq_num
              << ", payload_sum=" << m_payload_sum
              << ", lost_packets=" << m_lost_packets
              << ", discarded_packets=" << m_discarded_packets << std::endl;
    m_last_logged_packet_seq_num = m_last_packet_seq_num;
}