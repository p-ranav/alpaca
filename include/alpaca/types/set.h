#pragma once
#include <alpaca/detail/to_bytes.h>
#include <set>
#include <system_error>
#include <unordered_set>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_set_type(const T &input, std::vector<uint8_t> &bytes) {
  // save set size
  to_bytes_router(input.size(), bytes);

  // save values in set
  for (const auto &value : input) {
    using decayed_key_type = typename std::decay<typename T::value_type>::type;
    to_bytes_router<decayed_key_type>(value, bytes);
  }
}

template <typename T, typename U>
void pack(T &bytes, const std::set<U> &input) {
  to_bytes_from_set_type(input, bytes);
}

template <typename T, typename U>
void pack(T &bytes, const std::unordered_set<U> &input) {
  to_bytes_from_set_type(input, bytes);
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T>
void from_bytes_to_set(T &set, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index,
                       std::error_code &error_code) {
  // current byte is the size of the set
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  if (size > bytes.size() - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return;
  }

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    typename T::value_type value{};
    from_bytes_router(value, bytes, current_index, error_code);
    set.insert(value);
  }
}

template <typename T>
bool unpack(std::set<T> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
    from_bytes_to_set(output, bytes, byte_index, error_code);
    return true;
}

template <typename T>
bool unpack(std::unordered_set<T> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
    from_bytes_to_set(output, bytes, byte_index, error_code);
    return true;
}

}

}