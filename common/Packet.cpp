#include "Packet.h"
#include "Helpers.hpp"

#include <boost/endian/conversion.hpp>

Packet::Packet(SeqNumType seq_num, StreamKeyType stream_key, std::vector<PayloadItem> payload)
    : m_header { seq_num, stream_key }
    , m_payload(std::move(payload))
{
}

SeqNumType Packet::SeqNum() const noexcept
{
    return m_header.seq_num;
}

StreamKeyType Packet::StreamKey() const noexcept
{
    return m_header.stream_key;
}

std::vector<PayloadItem> Packet::Payload() const noexcept
{
    return m_payload;
}

std::vector<std::byte> Packet::Serialize() const noexcept
{
    const Header header { boost::endian::native_to_big(m_header.seq_num),
        boost::endian::native_to_big(m_header.stream_key), boost::endian::native_to_big(m_payload.size()) };

    return SerializeHelper(&header, sizeof(header),
        m_payload.data(), m_payload.size());
}

Packet Packet::Deserialize(const std::vector<std::byte>& data)
{
    Packet::Header header;
    const size_t deserialized = DerializeHelper(data,
        &header, sizeof(header));

    std::vector<PayloadItem> payload;

    header.payload_size = boost::endian::big_to_native(header.payload_size);
    payload.resize(header.payload_size);
    DerializeHelper(data.data() + deserialized, data.size(),
        payload.data(), payload.size());
    return Packet(boost::endian::big_to_native(header.seq_num),
        boost::endian::big_to_native(header.stream_key),
        std::move(payload));
}