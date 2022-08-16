#pragma once

#include "Helpers.h"

#include <string>

class Offer {
public:
    static constexpr size_t s_secret_key_size = 16;

    enum class EType : uint32_t {
        Undefined,
        Publisher
    };

private:
    struct SerializableOffer {
        Offer::EType type;
        char secret_key[Offer::s_secret_key_size];
        StreamKeyType stream_key;

        SerializableOffer() = default;
        SerializableOffer(Offer::EType type, std::string_view secret_key, StreamKeyType stream_key);
    };

public:
    static constexpr size_t s_serialized_size = sizeof(SerializableOffer);

    Offer(EType type, std::string_view secret_key);

    bool Validate(std::string_view expected_secret_key, EType expected_type) const noexcept;

    EType Type() const noexcept;
    std::string_view SecretKey() const noexcept;
    StreamKeyType StreamKey() const noexcept;

    std::vector<std::byte> Serialize() const noexcept;

    static Offer Deserialize(const std::vector<std::byte>& data);

private:
    Offer(SerializableOffer data);

    static StreamKeyType GenerateStreamKey();

private:
    const SerializableOffer m_data;
};
