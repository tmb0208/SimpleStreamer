#include "Offer.h"
#include "Helpers.h"

#include <sstream>
#include <vector>

#include <boost/endian/conversion.hpp>

Offer::Offer(EType type, std::string_view secret_key)
    : Offer(type, secret_key, GenerateStreamKey())
{
}

Offer::Offer(EType type, std::string_view secret_key, StreamKeyType stream_key)
    : m_type(type)
    , m_secret_key(secret_key)
    , m_stream_key(stream_key)
{
    if (m_secret_key.size() > s_secret_key_size) {
        std::stringstream err;
        err << "Secret key is longer than "
            << Offer::s_secret_key_size
            << ": " << m_secret_key
            << '(' << m_secret_key.size() << ')';
        throw std::runtime_error(err.str());
    }
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
    const auto type = boost::endian::native_to_big(m_type);
    std::string secret_key = m_secret_key;
    secret_key.resize(s_secret_key_size, '\0');
    const auto stream_key = boost::endian::native_to_big(m_stream_key);
    return ::SerializeHelper(&type, sizeof(type),
        secret_key.data(), secret_key.size(),
        &stream_key, sizeof(stream_key));
}

Offer Offer::Deserialize(const std::vector<std::byte>& data)
{
    EType type = EType::Undefined;
    std::string secret_key;
    secret_key.resize(s_secret_key_size);
    StreamKeyType stream_key = 0;

    ::DeserializeHelper(data,
        &type, sizeof(type),
        secret_key.data(), secret_key.size(),
        &stream_key, sizeof(stream_key));

    const auto last_redundunt_zero_index = secret_key.find_first_of('\0');
    secret_key.resize(last_redundunt_zero_index);

    return Offer(boost::endian::big_to_native(type),
        secret_key,
        boost::endian::big_to_native(stream_key));
}

StreamKeyType Offer::GenerateStreamKey()
{
    return static_cast<StreamKeyType>(rand());
}
