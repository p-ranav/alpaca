#pragma once
#include <serialize/detail/type.h>

namespace detail {

template <bool attempt_compression = true>
type get_repr_type(const uint64_t &input) {

  if constexpr (!attempt_compression) {
    return type::uint64;
  }

  if (input <= std::numeric_limits<uint8_t>::max() &&
      input >= std::numeric_limits<uint8_t>::min()) {
    return type::uint64_as_uint8;
  } else if (input <= std::numeric_limits<uint16_t>::max() &&
             input >= std::numeric_limits<uint16_t>::min()) {
    return type::uint64_as_uint16;
  } else if (input <= std::numeric_limits<uint32_t>::max() &&
             input >= std::numeric_limits<uint32_t>::min()) {
    return type::uint64_as_uint32;
  } else {
    return type::uint64;
  }
}

template <bool attempt_compression = true>
type get_repr_type(const uint32_t &input) {

  if constexpr (!attempt_compression) {
    return type::uint32;
  }

  if (input <= std::numeric_limits<uint8_t>::max() &&
      input >= std::numeric_limits<uint8_t>::min()) {
    return type::uint32_as_uint8;
  } else if (input <= std::numeric_limits<uint16_t>::max() &&
             input >= std::numeric_limits<uint16_t>::min()) {
    return type::uint32_as_uint16;
  } else {
    return type::uint32;
  }
}

template <bool attempt_compression = true>
type get_repr_type(const uint16_t &input) {
  if constexpr (!attempt_compression) {
    return type::uint16;
  }

  if (input <= std::numeric_limits<uint8_t>::max() &&
      input >= std::numeric_limits<uint8_t>::min()) {
    return type::uint16_as_uint8;
  } else {
    return type::uint16;
  }
}

template <bool attempt_compression = true>
type get_repr_type(const int64_t &input) {
  if constexpr (!attempt_compression) {
    return type::int64;
  }

  if (input <= std::numeric_limits<int8_t>::max() &&
      input >= std::numeric_limits<int8_t>::min()) {
    return type::int64_as_int8;
  } else if (input <= std::numeric_limits<int16_t>::max() &&
             input >= std::numeric_limits<int16_t>::min()) {
    return type::int64_as_int16;
  } else if (input <= std::numeric_limits<int32_t>::max() &&
             input >= std::numeric_limits<int32_t>::min()) {
    return type::int64_as_int32;
  } else {
    return type::int64;
  }
}

template <bool attempt_compression = true>
type get_repr_type(const int32_t &input) {
  if constexpr (!attempt_compression) {
    return type::int32;
  }

  if (input <= std::numeric_limits<int8_t>::max() &&
      input >= std::numeric_limits<int8_t>::min()) {
    return type::int32_as_int8;
  } else if (input <= std::numeric_limits<int16_t>::max() &&
             input >= std::numeric_limits<int16_t>::min()) {
    return type::int32_as_int16;
  } else {
    return type::int32;
  }
}

template <bool attempt_compression = true>
type get_repr_type(const int16_t &input) {
  if constexpr (!attempt_compression) {
    return type::int16;
  }

  if (input <= std::numeric_limits<int8_t>::max() &&
      input >= std::numeric_limits<int8_t>::min()) {
    return type::int16_as_int8;
  } else {
    return type::int16;
  }
}

} // namespace detail