#pragma once
#include <alpaca/detail/type_traits.h>
#include <alpaca/detail/variable_length_encoding.h>
#include <iterator>

namespace alpaca {

namespace detail {

static inline
void pack_crc32(std::vector<uint8_t> &bytes, const uint32_t &value) {
  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

// write as is
template <typename T, typename U>
typename std::enable_if<
  std::is_same_v<U, bool> ||
  std::is_same_v<U, char> ||
  std::is_same_v<U, uint8_t> ||
  std::is_same_v<U, uint16_t> ||
  std::is_same_v<U, int8_t> ||
  std::is_same_v<U, int16_t> ||
  std::is_same_v<U, float> ||
  std::is_same_v<U, double>, void>::type
pack(T &bytes, const U &value) {
  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

// encode as variable-length 
template <typename T, typename U>
typename std::enable_if<
  std::is_same_v<U, uint32_t> ||
  std::is_same_v<U, uint64_t> ||
  std::is_same_v<U, int32_t> ||
  std::is_same_v<U, int64_t> ||
  std::is_same_v<U, std::size_t>, void>::type
pack(T &bytes, const U &value) {
  encode_varint<U>(value, bytes);
}

// enum class
template <typename T, typename U>
typename std::enable_if<std::is_enum<U>::value, void>::type
pack(T &bytes, const U &value) {
  using underlying_type = typename std::decay<typename std::underlying_type<U>::type>::type;
  pack<T, underlying_type>(bytes, static_cast<underlying_type>(value));
}

} // namespace detail

} // namespace alpaca