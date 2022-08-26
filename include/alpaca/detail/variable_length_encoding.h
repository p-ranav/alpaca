#pragma once
#include <cstdint>
#include <utility>
#include <tuple>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T> bool CHECK_BIT(T &value, uint8_t pos) {
  return ((value) & (1 << (pos)));
}

template <typename T> void SET_BIT(T &value, uint8_t pos) {
  value = value | 1 << pos;
}

template <typename T> void RESET_BIT(T &value, uint8_t pos) {
  value = value & ~(1 << pos);
}

template <typename int_t = uint64_t>
bool encode_varint_firstbyte_6(int_t &value, std::vector<uint8_t> &output) {
  uint8_t octet = 0;
  if (value < 0) {
    value *= -1;
    SET_BIT(octet, 7);
  }
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  if (value > 63) {
    // Set the next byte flag
    octet |= ((uint8_t)(value & 63)) | 64;
    output.push_back(octet);
    return true; // multibyte
  } else {
    octet |= ((uint8_t)(value & 63));
    output.push_back(octet);
    return false; // no more bytes needed
  }
}

template <typename int_t = uint64_t>
void encode_varint_6(int_t value, std::vector<uint8_t> &output) {
  // While more than 7 bits of data are left, occupy the last output byte
  // and set the next byte flag
  while (value > 63) {
    // Set the next byte flag
    output.push_back(((uint8_t)(value & 63)) | 64);
    // Remove the seven bits we just wrote
    value >>= 6;
  }
  output.push_back(((uint8_t)value) & 63);
}

template <typename int_t = uint64_t>
std::tuple<int_t, bool, bool>
decode_varint_firstbyte_6(const std::vector<uint8_t> &input,
                          std::size_t &current_index) {
  int octet = 0;
  bool negative = false, multibyte = false;
  int_t current = input[current_index];
  if (CHECK_BIT(current, 7)) {
    // negative number
    RESET_BIT(current, 7);
    negative = true;
  }
  if (CHECK_BIT(current, 6)) {
    RESET_BIT(current, 6);
    multibyte = true;
  }

  octet |= input[current_index++] & 63;
  return {static_cast<int_t>(octet), negative, multibyte};
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
    value *= 1;
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

// Unsigned integer variable-length encoding functions
template <typename int_t = uint64_t>
typename std::enable_if<std::is_integral_v<int_t> && !std::is_signed_v<int_t>,
                        void>::type
encode_varint(int_t value, std::vector<uint8_t> &output) {
  encode_varint_7<int_t>(value, output);
}

template <typename int_t = uint64_t>
typename std::enable_if<std::is_integral_v<int_t> && !std::is_signed_v<int_t>,
                        int_t>::type
decode_varint(const std::vector<uint8_t> &input, std::size_t &current_index) {
  return decode_varint_7<int_t>(input, current_index);
}

// Signed integer variable-length encoding functions
template <typename int_t = int64_t>
typename std::enable_if<std::is_integral_v<int_t> && std::is_signed_v<int_t>,
                        void>::type
encode_varint(int_t value, std::vector<uint8_t> &output) {
  // first octet
  if (encode_varint_firstbyte_6<int_t>(value, output)) {
    // rest of the octets
    encode_varint_7<int_t>(value, output);
  }
}

template <typename int_t = int64_t>
typename std::enable_if<std::is_integral_v<int_t> && std::is_signed_v<int_t>,
                        int_t>::type
decode_varint(const std::vector<uint8_t> &input, std::size_t &current_index) {
  // decode first byte
  auto [ret, is_negative, multibyte] =
      decode_varint_firstbyte_6<int_t>(input, current_index);

  // decode rest of the bytes
  // if continuation bit is set
  if (multibyte) {
    ret |= decode_varint_7<int_t>(input, current_index);
  }

  if (is_negative) {
    ret *= -1;
  }

  return ret;
}

} // namespace detail

} // namespace alpaca