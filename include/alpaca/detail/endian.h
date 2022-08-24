#pragma once

namespace alpaca {

namespace detail {

static inline bool is_system_little_endian() {
  static int value{0x01};
  static auto address = static_cast<const void *>(&value);
  static auto least_significant_address =
      static_cast<const unsigned char *>(address);
  return (*least_significant_address == 0x01);
}

} // namespace detail

} // namespace alpaca