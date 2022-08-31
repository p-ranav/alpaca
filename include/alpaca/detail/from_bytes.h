#pragma once
#include <alpaca/detail/endian.h>
#include <alpaca/detail/options.h>
#include <alpaca/detail/variable_length_encoding.h>
#include <cstdint>
#include <iostream>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <options O, typename Container>
bool from_bytes_crc32(uint32_t &value, const Container &bytes,
                      std::size_t &current_index, std::size_t &end_index,
                      std::error_code &) {
  constexpr auto num_bytes_to_read = 4;

  if (end_index < num_bytes_to_read) {
    return false;
  }
  value = *(reinterpret_cast<const uint32_t *>(bytes.data() + current_index));
  update_value_based_on_alpaca_endian_rules<O, uint32_t>(value);
  current_index += num_bytes_to_read;
  return true;
}

// char, bool, small ints, float, double
// read as is
template <options O, typename T, typename Container>
typename std::enable_if<
    std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
        std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, char> || std::is_same_v<T, bool> ||
        std::is_same_v<T, float> || std::is_same_v<T, double>,
    bool>::type
from_bytes(T &value, const Container &bytes, std::size_t &current_index,
           std::size_t &end_index, std::error_code &error_code) {

  if (current_index >= end_index) {
    // end of input

    // default initialize the value
    value = T();

    // return true for forward compatibility
    return true;
  }

  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
    // there should be at least 1 byte
    if (end_index - current_index < 1) {
      error_code = std::make_error_code(std::errc::message_size);
      return false;
    }
  } else if constexpr (std::is_same_v<T, uint16_t> ||
                       std::is_same_v<T, int16_t>) {
    // there should be at least 2 bytes
    if (end_index - current_index < 2) {
      error_code = std::make_error_code(std::errc::message_size);
      return false;
    }
  }

  constexpr auto num_bytes_to_read = sizeof(T);
  if (end_index < num_bytes_to_read) {
    /// TODO: report error
    return false;
  }
  value = *(reinterpret_cast<const T *>(bytes.data() + current_index));
  current_index += num_bytes_to_read;
  update_value_based_on_alpaca_endian_rules<O, T>(value);
  return true;
}

// large ints
// decode variable-length encoding
template <options O, typename T, typename Container>
typename std::enable_if<
    std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t> ||
        std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t>,
    bool>::type
from_bytes(T &value, const Container &bytes, std::size_t &current_index,
           std::size_t &end_index, std::error_code &) {

  if (current_index >= end_index) {
    // end of input

    // default initialize the value
    value = T();

    // return true for forward compatibility
    return true;
  }

  constexpr auto use_fixed_length_encoding = (
      // Do not perform VLQ if:
      // 1. if the system is little-endian and the requested byte order is
      // big-endian
      // 2. if the system is big-endian and the requested byte order is
      // little-endian
      // 3. If fixed length encoding is requested
      (is_system_little_endian() &&
       enum_has_flag<options, O, options::big_endian>()) ||
      (is_system_big_endian() &&
       !enum_has_flag<options, O, options::big_endian>()) ||
      (enum_has_flag<options, O, options::fixed_length_encoding>()));

  if constexpr (use_fixed_length_encoding) {
    constexpr auto num_bytes_to_read = sizeof(T);
    if (end_index < num_bytes_to_read) {
      /// TODO: report error
      return false;
    }
    value = *(reinterpret_cast<const T *>(bytes.data() + current_index));
    current_index += num_bytes_to_read;
  } else {
    value = decode_varint<T>(bytes, current_index);
  }

  update_value_based_on_alpaca_endian_rules<O, T>(value);
  return true;
}

// enum class
template <options O, typename T, typename Container>
typename std::enable_if<std::is_enum_v<T>, bool>::type
from_bytes(T &value, const Container &bytes, std::size_t &current_index,
           std::size_t &end_index, std::error_code &error_code) {

  if (current_index >= end_index) {
    // end of input

    // default initialize the value
    value = T();

    // return true for forward compatibility
    return true;
  }

  using underlying_type = typename std::underlying_type<T>::type;
  underlying_type underlying_value{};
  from_bytes<O, underlying_type, Container>(
      underlying_value, bytes, current_index, end_index, error_code);
  value = static_cast<T>(underlying_value);
  return true;
}

} // namespace detail

} // namespace alpaca