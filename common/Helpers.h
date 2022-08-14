#include <cstddef>
#include <cstdint>
#include <limits>

constexpr int PORT = 1234;
constexpr size_t SECRET_SIZE = 256;
constexpr size_t STREAM_KEY_SIZE = 256;

using udp_port_type = uint32_t;

enum class OfferType : uint32_t { // TODO: Move to common
    Publisher,
    Undefined
};

struct Offer {
    OfferType offer_type = OfferType::Undefined;
    char secret[SECRET_SIZE] = {};
    char stream_key[STREAM_KEY_SIZE] = {};
};