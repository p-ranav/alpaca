#pragma once
#include <alpaca/detail/variable_length_encoding.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T, typename U>
// T is the type of field in the struct
// U is the type of data in the byte array
// e.g.,
//    T could be an int32_t, U could be a uint8_t
//    value = 5 is stored as a single byte even if the struct field is an
//    int32_t
bool read_bytes(T &value, const std::vector<uint8_t> &bytes,
                std::size_t &current_index,
                std::error_code &error_code) {
  constexpr auto num_bytes_to_read = sizeof(U);
  if (bytes.size() < num_bytes_to_read) {
    return false;
  }
  value = *(reinterpret_cast<const U *>(bytes.data() + current_index));
  current_index += num_bytes_to_read;
  return true;
}

template <typename T>
typename std::enable_if<std::is_same_v<T, bool>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
  // current byte is the value
  return read_bytes<bool, bool>(value, bytes, current_index, error_code);
}

template <typename T>
typename std::enable_if<std::is_same_v<T, char>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
  // current byte is the value
  return read_bytes<char, char>(value, bytes, current_index, error_code);
}

template <typename T>
typename std::enable_if<
    std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
        std::is_same_v<T, std::size_t> || std::is_same_v<T, int32_t> ||
        std::is_same_v<T, int64_t>,
    bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
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
  return read_bytes<T, T>(value, bytes, current_index, error_code);
}

template <typename T>
typename std::enable_if<std::is_same_v<T, float> || std::is_same_v<T, double>,
                        bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index, std::error_code &error_code) {
  return read_bytes<T, T>(value, bytes, current_index, error_code);
}

static inline bool from_bytes(std::string &value,
                              const std::vector<uint8_t> &bytes,
                              std::size_t &current_index,
                              std::error_code &error_code) {

  // current byte is the length of the string
  std::size_t size = decode_varint<std::size_t>(bytes, current_index);

  if (size > bytes.size() - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return false;
  }

  // read `size` bytes and save to value
  value.reserve(size);
  const uint8_t *ptr = bytes.data() + current_index;
  for (std::size_t i = 0; i < size; ++i) {
    value += ptr[i];
  }
  current_index += size;

  return true;
}

} // namespace detail

} // namespace alpaca