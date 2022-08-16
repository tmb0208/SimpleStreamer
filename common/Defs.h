#pragma once

#include <chrono>
#include <cstdint>
#include <limits>

constexpr uint16_t g_handshake_port = 1234;
constexpr size_t g_packets_per_minute = 15;
constexpr std::chrono::seconds g_gap_between_packets(60 / g_packets_per_minute);

using StreamKeyType = uint32_t;
using SeqNumType = uint32_t;
using PayloadItem = char;