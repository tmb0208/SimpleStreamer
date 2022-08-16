#pragma once

#include "Helpers.h"

#include <boost/asio.hpp>

class Publisher {
public:
    explicit Publisher(boost::asio::io_service& io_service);

    void Stream(std::string_view endpoint, boost::asio::ip::port_type port, std::istream& stream);

    StreamKeyType StreamKey() const noexcept;

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