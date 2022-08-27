#pragma once
#include <alpaca/detail/variable_length_encoding.h>
#include <cstdint>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

// from_bytes arithmetic value from bytearray
static inline bool from_bytes_crc32(uint32_t &value,
                                const std::vector<uint8_t> &bytes,
                                std::size_t &current_index, std::error_code &) {
  constexpr auto num_bytes_to_read = 4;

  if (bytes.size() < num_bytes_to_read) {
    return false;
  }
  value = *(reinterpret_cast<const uint32_t *>(bytes.data() + current_index));
  current_index += num_bytes_to_read;
  return true;
}

// char, bool, small ints, float, double
template <typename T>
typename std::enable_if<
  std::is_same_v<T, int8_t> ||
  std::is_same_v<T, int16_t> ||
  std::is_same_v<T, uint8_t> ||
  std::is_same_v<T, uint16_t> ||
  std::is_same_v<T, char> ||
  std::is_same_v<T, bool> ||
  std::is_same_v<T, float> ||
  std::is_same_v<T, double>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes, std::size_t &current_index,
       std::error_code & error_code) {

  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
    // there should be at least 1 byte
    if (bytes.size() - current_index < 1) {
      error_code = std::make_error_code(std::errc::message_size);
      return false;
    }
  } else if constexpr (std::is_same_v<T, uint16_t> ||
                       std::is_same_v<T, int16_t>) {
    // there should be at least 2 bytes
    if (bytes.size() - current_index < 2) {
      error_code = std::make_error_code(std::errc::message_size);
      return false;
    }
  }

  constexpr auto num_bytes_to_read = sizeof(T);
  if (bytes.size() < num_bytes_to_read) {
    return false;
  }
  value = *(reinterpret_cast<const T *>(bytes.data() + current_index));
  current_index += num_bytes_to_read;
  return true;
}

// large ints
template <typename T>
typename std::enable_if<
  std::is_same_v<T, int32_t> ||
  std::is_same_v<T, int64_t> ||
  std::is_same_v<T, uint32_t> ||
  std::is_same_v<T, uint64_t>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes, std::size_t &current_index,
       std::error_code &) {
  value = decode_varint<T>(bytes, current_index);
  return true;
}

} // namespace detail

} // namespace alpaca