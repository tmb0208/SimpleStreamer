#include "Offer.h"

#include <sstream>
#include <vector>

#include <boost/endian/conversion.hpp>

Offer::SerializableOffer::SerializableOffer(EType type, std::string_view secret_key, StreamKeyType stream_key)
    : type(type)
    , stream_key(stream_key)
{
    memcpy(this->secret_key, secret_key.data(), secret_key.size());
}

Offer::Offer(EType type, std::string_view secret_key)
    : m_data(type, secret_key, GenerateStreamKey())
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

Offer::Offer(SerializableOffer data)
    : m_data(std::move(data))
{
}

bool Offer::Validate(std::string_view expected_secret_key, EType expected_type) const noexcept // TODO: Move to Offer class
{
    return expected_secret_key == m_data.secret_key && expected_type == m_data.type;
}

Offer::EType Offer::Type() const noexcept
{
    return m_data.type;
}

std::string_view Offer::SecretKey() const noexcept
{
    return m_data.secret_key;
}

StreamKeyType Offer::StreamKey() const noexcept
{
    return m_data.stream_key;
}

std::vector<std::byte> Offer::Serialize() const noexcept
{
    SerializableOffer s(boost::endian::native_to_big(m_data.type),
        m_data.secret_key,
        boost::endian::native_to_big(m_data.stream_key));
    std::vector<std::byte> result(sizeof(s));
    memcpy(result.data(), &s, sizeof(s));
    return result;
}

Offer Offer::Deserialize(const std::vector<std::byte>& data)
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
    return Offer(std::move(res));
}

StreamKeyType Offer::GenerateStreamKey()
{
    return static_cast<StreamKeyType>(rand());
}
