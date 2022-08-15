#include "Publisher.h"

#include <iostream>

#include <boost/asio/buffer.hpp>
#include <boost/endian/conversion.hpp>

using namespace boost::asio;

Publisher::Publisher()
    : m_socket(m_io_service)
{
}

void Publisher::Stream(const char* secret_key)
{
    SendOffer(secret_key);
    std::cout << "Offer sent" << std::endl;

    const auto udp_port = WaitUdpPort();
    std::cout << "Port received: " << udp_port << std::endl;

    // TODO

    std::cout << "Completed" << std::endl;
}

void Publisher::GenerateStreamKey(char* stream_key, size_t size) const
{
    if (size == 0) {
        return;
    }

    while (size > 1) {
        *stream_key = '0' + rand() % 10;
        ++stream_key;
        --size;
    }

    *stream_key = '\0';
}

udp_port_type Publisher::WaitUdpPort()
{
    streambuf buf;
    boost::system::error_code error;
    read(m_socket, buf, transfer_exactly(sizeof(udp_port_type)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to read UDP port: " << error.message();
        throw std::runtime_error(err.str());
    }

    return *buffer_cast<const udp_port_type*>(buf.data());
}

void Publisher::SendOffer(const char* secret_key)
{
    if (secret_key == nullptr) {
        throw std::runtime_error("PUBLISHER_SECRET environment variable not initialized");
    }

    const size_t secret_key_size = strlen(secret_key);
    if (secret_key_size > sizeof(Offer::secret_key)) {
        std::stringstream err;
        err << "Secret key is longer than "
            << std::to_string(sizeof(Offer::secret_key))
            << ": " << secret_key
            << '(' << std::to_string(secret_key_size) << ')';
        throw std::runtime_error(err.str());
    }

    constexpr auto endpoint = "127.0.0.1";
    m_socket.connect(ip::tcp::endpoint(
        ip::address::from_string(endpoint), PORT));

    Offer offer { boost::endian::native_to_big(OfferType::Publisher), {}, {} };
    memcpy(&offer.secret_key, secret_key, secret_key_size + 1 /*\0*/);
    GenerateStreamKey(offer.stream_key, sizeof(offer.stream_key));
    const auto buf = buffer(&offer, sizeof(offer));
    boost::system::error_code error;
    write(m_socket, buf, transfer_exactly(sizeof(offer)), error);
    if (error) {
        std::stringstream err;
        err << "Failed to send offer: " << error.message();
        throw std::runtime_error(err.str());
    }
}
