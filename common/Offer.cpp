#include "Offer.h"

#include <sstream>
#include <vector>

#include <boost/endian/conversion.hpp>

namespace {
inline Offer::SerializableOffer Deserialize(const std::vector<std::byte>& data)
{
    Offer::SerializableOffer res;
    if (data.size() != sizeof(res)) {
        std::stringstream err;
        err << "Cannot deserialize: data size=" << data.size()
            << ", expected: " << sizeof(res);
        throw std::runtime_error(err.str());
    }

    memcpy(&res, data.data(), data.size());
    res.type = boost::endian::big_to_native(res.type);
    res.stream_key = boost::endian::big_to_native(res.stream_key);
    return res;
}
}

Offer::Offer(EType type, std::string_view secret_key)
    : m_type(type)
    , m_secret_key(secret_key)
    , m_stream_key(GenerateStreamKey())
{
    if (secret_key.size() > s_secret_key_size) {
        std::stringstream err;
        err << "Secret key is longer than "
            << Offer::s_secret_key_size
            << ": " << secret_key
            << '(' << secret_key.size() << ')';
        throw std::runtime_error(err.str());
    }
}

Offer::Offer(const std::vector<std::byte>& data)
    : m_type(::Deserialize(data).type)
    , m_secret_key(::Deserialize(data).secret_key)
    , m_stream_key(::Deserialize(data).stream_key)
{
}

bool Offer::Validate(std::string_view expected_secret_key, EType expected_type) const noexcept // TODO: Move to Offer class
{
    return expected_secret_key == m_secret_key && expected_type == m_type;
}

Offer::EType Offer::Type() const noexcept
{
    return m_type;
}

std::string_view Offer::SecretKey() const noexcept
{
    return m_secret_key;
}

StreamKeyType Offer::StreamKey() const noexcept
{
    return m_stream_key;
}

std::vector<std::byte> Offer::Serialize() const noexcept
{
    SerializableOffer s;
    s.type = boost::endian::native_to_big(m_type);
    memcpy(s.secret_key, m_secret_key.data(), m_secret_key.size());
    s.stream_key = boost::endian::native_to_big(m_stream_key);

    std::vector<std::byte> result(sizeof(s));
    memcpy(result.data(), &s, sizeof(s));
    return result;
}

Offer Offer::Deserialize(const std::vector<std::byte>& data)
{
    return Offer(data);
}

StreamKeyType Offer::GenerateStreamKey()
{
    return static_cast<StreamKeyType>(rand());
}
