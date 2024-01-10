#pragma once
#ifndef ALPACA_EXCLUDE_SUPPORT_STD_BITSET
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/type_info.h>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
typename std::enable_if<is_bitset<T>::value, void>::type
type_info(
    std::vector<uint8_t> &typeids,
    std::unordered_map<std::string_view, std::size_t> &struct_visitor_map) {
  typeids.push_back(to_byte<field_type::bitset>());
  using value_type = typename T::value_type;
  type_info<value_type>(typeids, struct_visitor_map);
}

template <options O, std::size_t N, typename Container>
void to_bytes_router(const std::bitset<N> &input, Container &bytes, std::size_t &byte_index);

template <options O, std::size_t N, typename Container>
void to_bytes_from_bitset_type(const std::bitset<N> &input, Container &bytes,
                               std::size_t &byte_index) {
  // save bitset size
  to_bytes_router<O, std::size_t>(input.size(), bytes, byte_index);

  // serialize the bitset itself into (bits / 8) bytes
  int num_bytes = input.size() / 8;
  for (int i=0; i < num_bytes; ++i) {
    uint8_t byte = 0;
    for (int bit = 0; bit < 8; ++bit) {
      int bit_index = byte * 8 + bit;
      if (bit_index >= input.size()) break;
      if (input[bit_index]) byte |= (1 << bit);
    }
    bytes[byte_index++] = byte;
  }
}

template <options O, typename Container, std::size_t N>
void to_bytes(Container &bytes, std::size_t &byte_index,
              const std::bitset<N> &input) {
  to_bytes_from_bitset_type<O>(input, bytes, byte_index);
}

template <options O, typename T, typename Container>
void from_bytes_router(T &output, Container &bytes, std::size_t &byte_index,
                       std::size_t &end_index, std::error_code &error_code);

template <options O, std::size_t N, typename Container>
bool from_bytes_to_bitset(std::bitset<N> &value, Container &bytes,
                          std::size_t &current_index, std::size_t &end_index,
                          std::error_code &error_code) {

  if (current_index >= end_index) {
    // end of input
    // return true for forward compatibility
    return true;
  }

  // current byte is the size of the vector
  std::size_t size = 0;
  detail::from_bytes<O, std::size_t>(size, bytes, current_index, end_index,
                                     error_code);

  if (size != N) {
    // the bitset we received is not the same size as the bitset we were given
    error_code = std::make_error_code(std::errc::invalid_argument);

    // stop here
    return false;
  }

  // we encode the number of bits as the size, but when we actually serialize
  // them we pack them, so we need to only deserialize size/8 bytes.
  std::size_t num_serialized_bytes = size / 8;

  if (num_serialized_bytes > end_index - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return false;
  }

  // reset the value to 0
  value.reset();

  // read `size` bits and save to value
  for (std::size_t byte_index = 0; byte_index < num_serialized_bytes; ++byte_index) {
    // get the byte at byte index
    uint8_t byte = bytes[byte_index];
    // loop over the bits
    for (int i=0; i<8; ++i) {
      int bit_index = byte_index * 8 + i;
      if (bit_index > size) break;
      value[bit_index] = static_cast<bool>(byte & (1 << i));
    }
  }

  return true;
}

template <options O, std::size_t N, typename Container>
bool from_bytes(std::bitset<N> &output, Container &bytes,
                std::size_t &byte_index, std::size_t &end_index,
                std::error_code &error_code) {
  return from_bytes_to_bitset<O>(output, bytes, byte_index, end_index,
                                 error_code);
}

} // namespace detail

} // namespace alpaca
#endif
