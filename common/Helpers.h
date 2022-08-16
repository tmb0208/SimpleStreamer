#pragma once

#include <chrono>
#include <cstdint>
#include <limits>

constexpr uint16_t g_handshake_port = 1234;
constexpr size_t g_packets_per_minute = 15;
constexpr std::chrono::seconds g_gap_between_packets(60 / g_packets_per_minute);

using StreamKeyType = uint32_t;
using SeqNumType = uint32_t;

struct Packet { // TODO: Rewrite as class
    static constexpr size_t s_max_payload_size = 100;

    struct Header {
        static constexpr SeqNumType s_invalid_seq_num = std::numeric_limits<SeqNumType>::max();

        SeqNumType seq_num = s_invalid_seq_num;
        StreamKeyType stream_key = 0;
    };

    using PayloadItem = char;

    Header header;
    PayloadItem payload[s_max_payload_size] = {};
};
