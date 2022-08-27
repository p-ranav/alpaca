#pragma once
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/from_bytes.h>
#include <string>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

static inline void to_bytes(const std::string &input,
                            std::vector<uint8_t> &bytes) {
  // save string length
  to_bytes_router(input.size(), bytes);

  for (auto &c : input) {
    append(bytes, c);
  }
}

template <typename T>
void append(T &bytes, const std::string &input) {
  to_bytes(input, bytes);
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

static inline
bool read_bytes(std::string &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
    from_bytes(output, bytes, byte_index, error_code);
    return true;
}

}

}