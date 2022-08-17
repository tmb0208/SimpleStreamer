#pragma once

#include <vector>

namespace {
size_t SerializeSizeHelperInternal();

template <typename... Args>
size_t SerializeSizeHelperInternal(const void* /*in*/, size_t in_size, Args... args);

void SerializeHelperInternal(std::byte* /*out*/, size_t /*max_size*/);

template <typename... Args>
void SerializeHelperInternal(std::byte* out, size_t max_size, const void* in, size_t in_size, Args... args);

template <typename... Args>
std::vector<std::byte> SerializeHelper(Args... args);

size_t DeserializeHelper(const std::byte* /*in*/, size_t /*size*/);

template <typename... Args>
size_t DeserializeHelper(const std::byte* in, size_t in_size, void* out, size_t out_size, Args... args);

template <typename... Args>
size_t DeserializeHelper(const std::vector<std::byte>& in, Args... args);
}

#include "Helpers.hpp"