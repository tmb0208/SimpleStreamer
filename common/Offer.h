#pragma once

#include "Defs.h"

#include <string>

#include <boost/core/span.hpp>

class Offer {
public:
    static constexpr size_t s_secret_key_size = 16;

    enum class EType : uint32_t {
        Undefined,
        Publisher
    };

    static constexpr size_t s_serialized_size = sizeof(EType) + s_secret_key_size + sizeof(StreamKeyType);

    Offer(EType type, std::string_view secret_key);

    bool Validate(std::string_view expected_secret_key, EType expected_type) const noexcept;

    EType Type() const noexcept;
    std::string_view SecretKey() const noexcept;
    StreamKeyType StreamKey() const noexcept;

    std::vector<Byte> Serialize() const noexcept;

    static Offer Deserialize(boost::span<const Byte> data);

private:
    Offer(EType type, std::string_view secret_key, StreamKeyType stream_key);

    static StreamKeyType GenerateStreamKey();

private:
    Offer::EType m_type;
    std::string m_secret_key;
    StreamKeyType m_stream_key;
};
