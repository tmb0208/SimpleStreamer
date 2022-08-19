#pragma once

#include "Defs.h"

#include <boost/core/span.hpp>

#include <type_traits>
#include <vector>

namespace helpers {
template <typename T>
size_t SizeOf(const T& v);

template <typename T, typename... Args>
size_t SizeOf(const T& v, const Args&... args);

template <typename T>
boost::span<Byte> SerializeInternal(boost::span<Byte> out, const T& v);

template <typename T, typename... Args>
boost::span<Byte> SerializeInternal(boost::span<Byte> out, const T& v, const Args&... args);

template <typename... Args>
std::vector<Byte> Serialize(const Args&... args);

template <typename T>
boost::span<const Byte> Deserialize(boost::span<const Byte> in, T& out);

template <typename T, typename... Args>
boost::span<const Byte> Deserialize(boost::span<const Byte> in, T& out, Args&... args);
}

#include "Helpers.hpp"