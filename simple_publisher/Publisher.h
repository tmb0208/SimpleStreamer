#pragma once

#include "Helpers.h"

#include <boost/asio.hpp>

class Publisher {
public:
    explicit Publisher(boost::asio::io_service& io_service,
        const boost::asio::ip::address& address,
        boost::asio::ip::port_type port,
        StreamKeyType stream_key);

    void Stream(std::istream& stream);

private:
    void SendPackets(std::istream& stream);
    void SendPacket(const Packet& packet, size_t payload_size);
    void Log(bool force = false) const noexcept;

private:
    boost::asio::io_service& m_io_service;
    boost::asio::ip::udp::socket m_socket;

    StreamKeyType m_stream_key;

    uint64_t m_payload_sum = 0;
    SeqNumType m_last_packet_seq_num = 0;
};