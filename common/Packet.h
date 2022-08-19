#pragma once

#include "Defs.h"

#include <vector>

#include <boost/core/span.hpp>

class Packet {
    struct Header {
        SeqNumType seq_num = s_invalid_seq_num;
        StreamKeyType stream_key = 0;
        uint64_t payload_size = 0;
    };

public:
    static constexpr size_t s_max_payload_size = 100;
    static constexpr size_t s_max_packet_size = s_max_payload_size + sizeof(Header);
    static constexpr SeqNumType s_invalid_seq_num = std::numeric_limits<SeqNumType>::max();

private:
public:
    Packet(SeqNumType seq_num, StreamKeyType stream_key, std::vector<Byte> payload);

    SeqNumType SeqNum() const noexcept;
    StreamKeyType StreamKey() const noexcept;
    std::vector<Byte> Payload() const noexcept;

    std::vector<Byte> Serialize() const noexcept;
    static Packet Deserialize(boost::span<const Byte> data);

private:
    Header m_header;
    std::vector<Byte> m_payload;
};
