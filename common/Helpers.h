#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>

inline constexpr int g_handshake_port = 1234;
inline constexpr size_t g_packets_per_minute = 15;
inline constexpr std::chrono::seconds g_gap_between_packets(60 / g_packets_per_minute);

using StreamKeyType = uint32_t;
using SeqNumType = uint32_t;

enum class OfferType : uint32_t {
    Publisher,
    Undefined
};

struct Offer {
    inline static constexpr size_t s_secret_key_size = 16;

    OfferType offer_type = OfferType::Undefined;
    char secret_key[s_secret_key_size] = {};
    StreamKeyType stream_key = 0;
};

struct Packet {
    inline static constexpr size_t s_max_payload_size = 100;

    struct Header {
        inline static constexpr SeqNumType s_invalid_seq_num = std::numeric_limits<SeqNumType>::max();

        SeqNumType seq_num = s_invalid_seq_num;
        StreamKeyType stream_key = 0;
    };

    using PayloadItem = char;

    Header header;
    PayloadItem payload[s_max_payload_size] = {};
};
