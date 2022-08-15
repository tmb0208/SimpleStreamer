#pragma once

#include <boost/asio.hpp>

#include "Helpers.h"

class Publisher {
public:
    Publisher(std::shared_ptr<boost::asio::io_service> io_service);

    void Stream(const char* endpoint, boost::asio::ip::port_type port, const std::string& file_path);

    StreamKeyType StreamKey() const;

private:
    void SendPackets(const std::string& file_path);
    void SendPacket(const Packet &packet, size_t payload_size);
    void Log() const noexcept;

private:
    std::shared_ptr<boost::asio::io_service> m_io_service;
    boost::asio::ip::udp::socket m_socket;

    StreamKeyType m_stream_key;

    uint64_t m_payload_sum = 0;
    SeqNumType m_last_packet_seq_num = 0;

    inline static constexpr size_t s_packets_log_frequency = 5;
};