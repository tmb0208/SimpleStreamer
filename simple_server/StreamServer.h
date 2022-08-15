#pragma once

#include <memory>

#include <boost/asio.hpp>

#include "Helpers.h"

class StreamServer {
public:
    StreamServer(std::shared_ptr<boost::asio::io_service> io_service);

    void Run(const std::string& file_path, StreamKeyType stream_key);

    boost::asio::ip::port_type Port() const noexcept;

private:
    void WriteReceivedPacketsTo(const std::string& file_path, StreamKeyType stream_key);
    size_t ReceivePacket(Packet& result);
    void Log(StreamKeyType stream_key, bool force = false) const noexcept;

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;

    boost::asio::ip::udp::socket m_socket;

    size_t m_discarded_packets = 0;
    size_t m_lost_packets = 0;
    uint64_t m_payload_sum = 0;
    SeqNumType m_last_packet_seq_num = Packet::Header::s_invalid_seq_num;
    mutable SeqNumType m_last_logged_packet_seq_num = Packet::Header::s_invalid_seq_num;
};
