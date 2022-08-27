#pragma once
#include <alpaca/detail/type_traits.h>
#include <alpaca/detail/variable_length_encoding.h>
#include <iterator>

namespace alpaca {

namespace detail {

// write bytes from arithmetic value ot bytearray
template <typename T, typename U>
typename std::enable_if<std::is_arithmetic_v<U>, void>::type
append(T &bytes, const U &value) {
  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

// Integral types
template <typename T>
typename std::enable_if<std::is_integral_v<T>, void>::type
to_bytes(const T &input, std::vector<uint8_t> &bytes) {

  // for smaller ints, save as is
  // don't perform variable-length encoding
  if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
                std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t>) {
    append(bytes, input);
  } else {
    encode_varint<T>(input, bytes);
  }
}

// bool
static inline void to_bytes(const bool &input, std::vector<uint8_t> &bytes) {
  append(bytes, input);
}

// char
static inline void to_bytes(const char &input, std::vector<uint8_t> &bytes) {
  to_bytes(static_cast<uint8_t>(input), bytes);
}

// float
static inline void to_bytes(const float &input, std::vector<uint8_t> &bytes) {
  append(bytes, input);
}

// double
static inline void to_bytes(const double &input, std::vector<uint8_t> &bytes) {
  append(bytes, input);
}

} // namespace detail

} // namespace alpaca