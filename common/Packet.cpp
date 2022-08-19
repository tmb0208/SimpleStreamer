#include "Packet.h"
#include "Helpers.h"

#include <boost/endian/conversion.hpp>

Packet::Packet(SeqNumType seq_num, StreamKeyType stream_key, std::vector<Byte> payload)
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

std::vector<Byte> Packet::Payload() const noexcept
{
    return m_payload;
}

std::vector<Byte> Packet::Serialize() const noexcept
{
    const Header header { boost::endian::native_to_big(m_header.seq_num),
        boost::endian::native_to_big(m_header.stream_key), boost::endian::native_to_big(m_payload.size()) };

    return helpers::Serialize(header, m_payload);
}

Packet Packet::Deserialize(boost::span<const Byte> data)
{
    Packet::Header header;
    const auto payload_data = helpers::Deserialize(data, header);
    header.payload_size = boost::endian::big_to_native(header.payload_size);

    std::vector<Byte> payload(header.payload_size);
    helpers::Deserialize(payload_data, payload);
    return Packet(boost::endian::big_to_native(header.seq_num),
        boost::endian::big_to_native(header.stream_key),
        std::move(payload));
}