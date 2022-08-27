#pragma once
#include <alpaca/detail/variable_length_encoding.h>
#include <cstdint>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

// unpack arithmetic value from bytearray
static inline bool unpack_crc32(uint32_t &value,
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

// unpack arithmetic value from bytearray
template <typename T>
typename std::enable_if<std::is_arithmetic_v<T>, bool>::type
unpack(T &value, const std::vector<uint8_t> &bytes, std::size_t &current_index,
       std::error_code &) {
  constexpr auto num_bytes_to_read = sizeof(T);
  if (bytes.size() < num_bytes_to_read) {
    return false;
  }
  value = *(reinterpret_cast<const T *>(bytes.data() + current_index));
  current_index += num_bytes_to_read;
  return true;
}

// bool
template <typename T>
typename std::enable_if<std::is_same_v<T, bool>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
  // current byte is the value
  return unpack<bool>(value, bytes, current_index, error_code);
}

// char
template <typename T>
typename std::enable_if<std::is_same_v<T, char>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
  // current byte is the value
  return unpack<char>(value, bytes, current_index, error_code);
}

// larger ints - 32bit, 64bit
template <typename T>
typename std::enable_if<
    std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
        std::is_same_v<T, std::size_t> || std::is_same_v<T, int32_t> ||
        std::is_same_v<T, int64_t>,
    bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &) {
  value = decode_varint<T>(bytes, current_index);
  return true;
}

// small ints
template <typename T>
typename std::enable_if<
    std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t>,
    bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
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
  return unpack<T>(value, bytes, current_index, error_code);
}

// float and double
template <typename T>
typename std::enable_if<std::is_same_v<T, float> || std::is_same_v<T, double>,
                        bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
  return unpack<T>(value, bytes, current_index, error_code);
}

} // namespace detail

} // namespace alpaca