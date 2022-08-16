#pragma once

#include "Defs.h"

#include <vector>

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
    Packet(SeqNumType seq_num, StreamKeyType stream_key, std::vector<PayloadItem> payload);

    SeqNumType SeqNum() const noexcept;
    StreamKeyType StreamKey() const noexcept;
    std::vector<PayloadItem> Payload() const noexcept;

    std::vector<std::byte> Serialize() const noexcept;
    static Packet Deserialize(const std::vector<std::byte>& data);

private:
    Header m_header;
    std::vector<PayloadItem> m_payload;
};
