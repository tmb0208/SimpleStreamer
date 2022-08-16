#pragma once

#include <cstring>
#include <stdexcept>
#include <vector>

size_t SerializeSizeHelperInternal()
{
    return 0;
}

template <typename... Args>
size_t SerializeSizeHelperInternal(const void* /*in*/, size_t in_size, Args... args)
{
    return in_size + SerializeSizeHelperInternal(std::forward<Args>(args)...);
}

void SerializeHelperInternal(std::byte* /*out*/, size_t /*max_size*/)
{
}

template <typename... Args>
void SerializeHelperInternal(std::byte* out, size_t max_size, const void* in, size_t in_size, Args... args)
{
    if (in_size > max_size) {
        throw std::runtime_error("Not enough space to serialize");
    }

    memcpy(out, in, in_size);
    SerializeHelperInternal(out + in_size, max_size - in_size, std::forward<Args>(args)...);
}

template <typename... Args>
std::vector<std::byte> SerializeHelper(Args... args)
{
    std::vector<std::byte> result;
    const size_t result_size = SerializeSizeHelperInternal(args...);
    result.resize(result_size);
    SerializeHelperInternal(result.data(), result.size(), args...);
    return result;
}

size_t DerializeHelper(const std::byte* /*in*/, size_t /*size*/)
{
    return 0;
}

template <typename... Args>
size_t DerializeHelper(const std::byte* in, size_t in_size, void* out, size_t out_size, Args... args)
{
    if (out_size > in_size) {
        throw std::runtime_error("Not enough space to deserialize");
    }

    memcpy(out, in, out_size);
    return out_size + DerializeHelper(in + in_size, in_size - out_size, std::forward<Args>(args)...);
}

template <typename... Args>
size_t DerializeHelper(const std::vector<std::byte>& in, Args... args)
{
    return DerializeHelper(in.data(), in.size(), std::forward<Args>(args)...);
}