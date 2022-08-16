#pragma once

#include "Helpers.h"

#include <memory>

#include <boost/asio.hpp>

class StreamServer {
public:
    explicit StreamServer(boost::asio::io_service& io_service);

    void Run(std::ostream& stream, StreamKeyType stream_key);

    boost::asio::ip::port_type Port() const noexcept;

private:
    void WriteReceivedPacketsTo(std::ostream& stream, StreamKeyType stream_key);
    size_t ReceivePacket(Packet& result);
    void Log(StreamKeyType stream_key, bool force = false) noexcept;

private:
    boost::asio::io_service& m_io_service;

    boost::asio::ip::udp::socket m_socket;

    size_t m_discarded_packets = 0;
    size_t m_lost_packets = 0;
    uint64_t m_payload_sum = 0;
    SeqNumType m_last_packet_seq_num = Packet::Header::s_invalid_seq_num;
    SeqNumType m_last_logged_packet_seq_num = Packet::Header::s_invalid_seq_num;
};
