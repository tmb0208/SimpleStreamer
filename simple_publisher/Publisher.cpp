#include "Publisher.h"
#include "Packet.h"

#include <iostream>

#include <boost/endian/conversion.hpp>

using namespace boost::asio;

Publisher::Publisher(boost::asio::io_service& io_service,
    const ip::address& address,
    boost::asio::ip::port_type port,
    StreamKeyType stream_key)
    : m_io_service(io_service)
    , m_socket(m_io_service)
    , m_stream_key(stream_key)
{
    m_socket.connect(ip::udp::endpoint(address, port));
}

void Publisher::Stream(std::istream& stream)
{
    std::cout << "Stream started" << std::endl;
    SendPackets(stream);
    std::cout << "Stream completed" << std::endl;
}

void Publisher::SendPackets(std::istream& stream)
{
    while (stream.good()) {
        std::vector<PayloadItem> payload(Packet::s_max_payload_size);
        stream.read(payload.data(), Packet::s_max_payload_size);
        const auto bytes_read = stream.gcount();
        payload.resize(bytes_read);

        Packet packet(m_last_packet_seq_num, m_stream_key, std::move(payload));
        SendPacket(packet);
        Log();
        ++m_last_packet_seq_num;
        //        std::this_thread::sleep_for(g_gap_between_packets);
    }

    Log(true);
}

void Publisher::SendPacket(const Packet& packet)
{
    boost::system::error_code error;
    m_socket.send(buffer(packet.Serialize()), {}, error);
    if (error) {
        detail::throw_error(error, "Failed to send packets");
    }

    m_payload_sum += packet.Payload().size();
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
