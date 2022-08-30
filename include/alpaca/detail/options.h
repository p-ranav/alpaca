#pragma once
#include <type_traits>

namespace alpaca {

enum class options {
  none = 0,
  big_endian = 1,
  fixed_length_encoding = 2,
  with_version = 4,
  with_checksum = 8,
  unchecked = 16
};

template <typename E> struct enable_bitmask_operators {
  static constexpr bool enable = false;
};

template <typename E>
constexpr typename std::enable_if<enable_bitmask_operators<E>::enable, E>::type
operator|(E lhs, E rhs) {
  using underlying = typename std::underlying_type<E>::type;
  return static_cast<E>(static_cast<underlying>(lhs) |
                        static_cast<underlying>(rhs));
}

template <typename T, T value, T flag> constexpr bool enum_has_flag() {
  using underlying = typename std::underlying_type<T>::type;
  return (static_cast<underlying>(value) & static_cast<underlying>(flag)) ==
         static_cast<underlying>(flag);
}

template <> struct enable_bitmask_operators<options> {
  static constexpr bool enable = true;
};

} // namespace alpaca