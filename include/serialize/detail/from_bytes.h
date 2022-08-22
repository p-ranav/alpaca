#pragma once
#include <cstdint>
#include <serialize/detail/unsigned_int_encoding.h>
#include <vector>

#include <iostream>

namespace detail {

template <typename T, typename U>
// T is the type of field in the struct
// U is the type of data in the byte array
// e.g.,
//    T could be an int32_t, U could be a uint8_t
//    value = 5 is stored as a single byte even if the struct field is an
//    int32_t
bool read_bytes(T &value, const std::vector<uint8_t> &bytes,
                std::size_t &current_index) {
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
           std::size_t &current_index) {
  // current byte is the value
  return read_bytes<bool, bool>(value, bytes, current_index);
}

template <typename T>
typename std::enable_if<std::is_same_v<T, char>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index) {
  // current byte is the value
  return read_bytes<char, char>(value, bytes, current_index);
}

template <typename T>
typename std::enable_if<
    std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
        std::is_same_v<T, std::size_t>,
    bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index) {
  value = static_cast<T>(decode_varint(bytes, current_index));
  return true;
}

template <typename T, bool handle_compression = true>
typename std::enable_if<
    std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
        std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>,
    bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index) {

  if constexpr (!handle_compression) {
    return read_bytes<T, T>(value, bytes, current_index);
  }

  // current byte is the width of the value
  type integer_value_type = static_cast<type>(bytes[current_index++]);

  if (integer_value_type == type::int8) {
    return read_bytes<T, int8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int16_as_int8) {
    return read_bytes<T, int8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int16) {
    return read_bytes<T, int16_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int32_as_int8) {
    return read_bytes<T, int8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int32_as_int16) {
    return read_bytes<T, int16_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int32) {
    return read_bytes<T, int32_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int64_as_int8) {
    return read_bytes<T, int8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int64_as_int16) {
    return read_bytes<T, int16_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int64_as_int32) {
    return read_bytes<T, int32_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int64) {
    return read_bytes<T, int64_t>(value, bytes, current_index);
  } else {
    return false;
  }
}

template <typename T>
typename std::enable_if<std::is_same_v<T, float> || std::is_same_v<T, double>,
                        bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index) {
  return read_bytes<T, T>(value, bytes, current_index);
}

static inline bool from_bytes_to_string(std::string &value,
                                        const std::vector<uint8_t> &bytes,
                                        std::size_t &current_index) {

  // current byte is the length of the string
  std::size_t size = decode_varint<std::size_t>(bytes, current_index);

  // read `size` bytes and save to value
  value.reserve(size);
  const uint8_t *ptr = bytes.data() + current_index;
  for (std::size_t i = 0; i < size; ++i) {
    value += ptr[i];
  }

  return true;
}

template <typename T>
bool from_bytes_to_vector(std::vector<T> &value,
                          const std::vector<uint8_t> &bytes,
                          std::size_t &current_index) {

  // current byte is the size of the vector
  std::size_t size = decode_varint<std::size_t>(bytes, current_index);

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    T v;

    // check if signed integer type
    // no compression in this case
    if constexpr (std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
                  std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>) {
      from_bytes<T, false>(v, bytes, current_index);
    }
    /// TODO: handle nested vectors, strings, and other container types
    else {
      from_bytes<T>(v, bytes, current_index);
    }

    value.push_back(v);
  }

  return true;
}

} // namespace detail