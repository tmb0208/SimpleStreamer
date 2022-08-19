#pragma once

#include "Helpers.h"

namespace helpers {
template <typename T>
size_t SizeOf(const T& v)
{
    if constexpr (std::is_trivially_copyable_v<T>)
        return sizeof(T);
    else
        return std::size(v);
}

template <typename T, typename... Args>
size_t SizeOf(const T& v, const Args&... args)
{
    return SizeOf(v) + SizeOf(args...);
}

template <typename T>
boost::span<Byte> SerializeInternal(boost::span<Byte> out, const T& v)
{
    auto size = SizeOf(v);
    if (out.size() < size) {
        throw std::runtime_error("Not enough space to serialize");
    }

    if constexpr (std::is_trivially_copyable_v<T>) {
        memcpy(out.data(), &v, size);
    } else {
        std::copy(std::begin(v), std::end(v), out.begin());
    }

    return out.subspan(size);
}

template <typename T, typename... Args>
boost::span<Byte> SerializeInternal(boost::span<Byte> out, const T& v, const Args&... args)
{
    out = SerializeInternal(out, v);
    return SerializeInternal(out, args...);
}

template <typename... Args>
std::vector<Byte> Serialize(const Args&... args)
{
    size_t result_size = SizeOf(args...);
    std::vector<Byte> result(result_size);
    auto span = SerializeInternal(result, args...);
    return result;
}

template <typename T>
boost::span<const Byte> Deserialize(boost::span<const Byte> in, T& out)
{
    auto size = SizeOf(out);
    if (in.size() < size) {
        throw std::runtime_error("Not enough space to deserialize");
    }

    if constexpr (std::is_trivially_copyable_v<T>) {
        memcpy(&out, in.data(), size);
    } else {
        std::copy(std::begin(in), std::begin(in) + size, out.begin());
    }

    return in.subspan(size);
}

template <typename T, typename... Args>
boost::span<const Byte> Deserialize(boost::span<const Byte> in, T& out, Args&... args)
{
    in = Deserialize(in, out);
    return Deserialize(in, args...);
}
}
