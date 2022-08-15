#include <cstddef>
#include <cstdint>
#include <limits>

inline constexpr int g_handshake_port = 1234;

enum class OfferType : uint32_t { // TODO: Move to common
    Publisher,
    Undefined
};

struct Offer {
    inline static constexpr size_t s_secret_key_size = 16;

    OfferType offer_type = OfferType::Undefined;
    char secret_key[s_secret_key_size] = {};
    uint32_t stream_key = 0;
};

struct Packet {
    inline static constexpr size_t s_max_payload_size = 100;

    struct Header {
        uint32_t seq_num = std::numeric_limits<uint32_t>::max();
        decltype(Offer::stream_key) stream_key = 0;
    };

    Header header;
    uint8_t payload[s_max_payload_size] = {};
};
