#pragma once
#include <cstdint>
#include <vector>

namespace detail {

template <typename int_t = uint64_t>
void encode_varint(int_t value, std::vector<uint8_t> &output) {
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (value > 127) {
    //|128: Set the next byte flag
    output.push_back(((uint8_t)(value & 127)) | 128);
    // Remove the seven bits we just wrote
    value >>= 7;
  }
  output.push_back(((uint8_t)value) & 127);
}

template <typename int_t = uint64_t>
int_t decode_varint(const std::vector<uint8_t> &input,
                    std::size_t &current_index) {
  int_t ret = 0;
  for (std::size_t i = 0; i < sizeof(int_t); ++i) {
    ret |= (static_cast<int_t>(input[current_index + i] & 127)) << (7 * i);
    // If the next-byte flag is set
    if (!(input[current_index + i] & 128)) {
      current_index += i + 1;
      break;
    }
  }
  return ret;
}

} // namespace detail