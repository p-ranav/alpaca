#pragma once
#include <cstdint>
#include <serialize/detail/size_to_type.h>
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
typename std::enable_if<
    std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
        std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
        std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
        std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>,
    bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes,
           std::size_t &current_index) {
  // current byte is the width of the value
  type integer_value_type = static_cast<type>(bytes[current_index++]);

  if (integer_value_type == type::uint8) {
    return read_bytes<T, uint8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint16_as_uint8) {
    return read_bytes<T, uint8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint16) {
    return read_bytes<T, uint16_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint32_as_uint8) {
    return read_bytes<T, uint8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint32_as_uint16) {
    return read_bytes<T, uint16_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint32) {
    return read_bytes<T, uint32_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint64_as_uint8) {
    return read_bytes<T, uint8_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint64_as_uint16) {
    return read_bytes<T, uint16_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint64_as_uint32) {
    return read_bytes<T, uint32_t>(value, bytes, current_index);
  } else if (integer_value_type == type::uint64) {
    return read_bytes<T, uint64_t>(value, bytes, current_index);
  } else if (integer_value_type == type::int8) {
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

static inline bool from_bytes_to_string(std::string &value,
                                        const std::vector<uint8_t> &bytes,
                                        std::size_t &current_index) {

  // current byte is the length of the string
  type size_type = static_cast<type>(bytes[current_index++]);
  std::size_t size{0};
  bool read_result{true};
  if (size_type == type::uint8) {
    read_result = read_bytes<std::size_t, uint8_t>(size, bytes, current_index);
  } else if (size_type == type::uint16_as_uint8) {
    read_result = read_bytes<std::size_t, uint8_t>(size, bytes, current_index);
  } else if (size_type == type::uint16) {
    read_result = read_bytes<std::size_t, uint16_t>(size, bytes, current_index);
  } else if (size_type == type::uint32_as_uint8) {
    read_result = read_bytes<std::size_t, uint8_t>(size, bytes, current_index);
  } else if (size_type == type::uint32_as_uint16) {
    read_result = read_bytes<std::size_t, uint16_t>(size, bytes, current_index);
  } else if (size_type == type::uint32) {
    read_result = read_bytes<std::size_t, uint32_t>(size, bytes, current_index);
  } else if (size_type == type::uint64_as_uint8) {
    read_result = read_bytes<std::size_t, uint8_t>(size, bytes, current_index);
  } else if (size_type == type::uint64_as_uint16) {
    read_result = read_bytes<std::size_t, uint16_t>(size, bytes, current_index);
  } else if (size_type == type::uint64_as_uint32) {
    read_result = read_bytes<std::size_t, uint32_t>(size, bytes, current_index);
  } else if (size_type == type::uint64) {
    read_result = read_bytes<std::size_t, uint64_t>(size, bytes, current_index);
  } else {
    // using type::size* for the size
    size = type_to_size(size_type);
  }

  std::cout << size << "\n";

  if (!read_result) {
    return false;
  }

  // read `size` bytes and save to value
  value.reserve(size);
  const uint8_t *ptr = bytes.data() + current_index;
  for (std::size_t i = 0; i < size; ++i) {
    value += ptr[i];
  }

  return true;
}

} // namespace detail