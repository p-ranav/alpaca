#pragma once
#include <cstdint>
#include <pair>
#include <vector>
#include <iostream>

namespace detail {

template <typename int_t = uint64_t>
void encode_varint_firstbyte_6(int_t& value, std::vector<uint8_t> &output) {
  uint8_t octet = 0;
  if (value < 0) {
    SET_BIT(octet, 7);
  }
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  if (std::abs(value) > 63) {
    //|128: Set the next byte flag
    octet |= ((uint8_t)(std::abs(value) & 63)) | 64;
    output.push_back(octet);
  }
}

template <typename int_t = uint64_t>
void encode_varint_6(int_t value, std::vector<uint8_t> &output) {
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (value > 63) {
    //|128: Set the next byte flag
    output.push_back(((uint8_t)(value & 63)) | 64);
    // Remove the seven bits we just wrote
    value >>= 6;
  }
  output.push_back(((uint8_t)value) & 63);
}

template <typename int_t = uint64_t>
std::pair<int_t, bool> decode_varint_firstbyte_6(std::vector<uint8_t> &input,
                    std::size_t &current_index) {
  uint8_t octet = 0;
  bool negative = false;
  if (CHECK_BIT(input[current_index], 7)) {
    // negative number
    RESET_BIT(input[current_index], 7);
    negative = true;
  }
  RESET_BIT(input[current_index], 6);
  octet |= input[current_index++] & 63;
  return {static_cast<int_t>(octet), negative};
}

template <typename int_t = uint64_t>
int_t decode_varint_6(const std::vector<uint8_t> &input,
                    std::size_t &current_index) {
  int_t ret = 0;
  for (std::size_t i = 0; i < sizeof(int_t); ++i) {
    ret |= (static_cast<int_t>(input[current_index + i] & 63)) << (6 * i);
    // If the next-byte flag is set
    if (!(input[current_index + i] & 64)) {
      current_index += i + 1;
      break;
    }
  }
  return ret;
}

template <typename int_t = uint64_t>
void encode_varint_7(int_t value, std::vector<uint8_t> &output) {
  if (value < 0) {
    value = std::abs(value);
  }
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
int_t decode_varint_7(const std::vector<uint8_t> &input,
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

}
