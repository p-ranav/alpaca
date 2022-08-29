#pragma once
#include <alpaca/detail/endian.h>
#include <alpaca/detail/variable_length_encoding.h>
#include <iterator>

namespace alpaca {

namespace detail {

template <options O>
void to_bytes_crc32(std::vector<uint8_t> &bytes,
                    const uint32_t &original_value) {

  uint32_t value = original_value;

  // if system is little endian
  // but big_endian is requested
  if constexpr (__BYTE_ORDER == __ALPACA_LITTLE_ENDIAN &&
                enum_has_flag<options, O, options::big_endian>()) {
    value = byte_swap<uint32_t, byte_order::big_endian>(value);
  }

  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

// write as is
template <options O, typename T, typename U>
typename std::enable_if<
    std::is_same_v<U, bool> || std::is_same_v<U, char> ||
        std::is_same_v<U, uint8_t> || std::is_same_v<U, uint16_t> ||
        std::is_same_v<U, int8_t> || std::is_same_v<U, int16_t> ||
        std::is_same_v<U, float> || std::is_same_v<U, double>,
    void>::type
to_bytes(T &bytes, const U &original_value) {

  U value = original_value;
  update_value_based_on_alpaca_endian_rules<O, U>(value);

  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

// encode as variable-length
template <options O, typename T, typename U>
typename std::enable_if<
    std::is_same_v<U, uint32_t> || std::is_same_v<U, uint64_t> ||
        std::is_same_v<U, int32_t> || std::is_same_v<U, int64_t> ||
        std::is_same_v<U, std::size_t>,
    void>::type
to_bytes(T &bytes, const U &original_value) {

  U value = original_value;
  update_value_based_on_alpaca_endian_rules<O, U>(value);

  // If big-endian is requested, dont encode as VLQ
  // If fixed-length encoding is requested, dont encode as VLQ
  if constexpr (enum_has_flag<options, O, options::big_endian>() ||
                enum_has_flag<options, O, options::fixed_length_encoding>()) {
    std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
              static_cast<const char *>(static_cast<const void *>(&value)) +
                  sizeof value,
              std::back_inserter(bytes));
  } else {
    encode_varint<U>(value, bytes);
  }
}

// enum class
template <options O, typename T, typename U>
typename std::enable_if<std::is_enum<U>::value, void>::type
to_bytes(T &bytes, const U &value) {
  using underlying_type =
      typename std::decay<typename std::underlying_type<U>::type>::type;
  to_bytes<O, T, underlying_type>(bytes, static_cast<underlying_type>(value));
}

} // namespace detail

} // namespace alpaca