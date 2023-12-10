#pragma once
#ifndef ALPACA_EXCLUDE_SUPPORT_STD_FILESYSTEM_PATH
#include <alpaca/detail/from_bytes.h>
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/type_info.h>
#include <filesystem>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
typename std::enable_if<std::is_same<T, std::filesystem::path>::value,
                        void>::type
type_info(std::vector<uint8_t> &typeids,
          std::unordered_map<std::string_view, std::size_t> &) {
  typeids.push_back(to_byte<field_type::filesystem_path>());
}

template <options O, typename T, typename Container>
void to_bytes_router(const T &input, Container &bytes, std::size_t &byte_index);

template <options O, typename Container>
void to_bytes(Container &bytes, std::size_t &byte_index,
              const std::filesystem::path &input) {
  // save string length
  to_bytes_router<O>(input.native().size(), bytes, byte_index);

  for (const auto &c : input.native()) {
    to_bytes<O>(bytes, byte_index, c);
  }
}

template <options O, typename Container>
bool from_bytes(std::filesystem::path &value, Container &bytes,
           std::size_t &current_index, std::size_t &end_index,
           std::error_code &error_code) {

  if (current_index >= end_index) {
    // end of input
    // return true for forward compatibility
    return true;
  }

  // current byte is the length of the string
  std::size_t size = 0;
  detail::from_bytes<O, std::size_t>(size, bytes, current_index, end_index,
                                     error_code);

  if (size > end_index - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return false;
  }

  // read `size` bytes and save to value
  std::filesystem::path::string_type buff;
  using CharType = std::filesystem::path::value_type;

  buff.reserve(size * sizeof(CharType));

  for (std::size_t i = 0; i < size; ++i) {
    CharType character{};
    from_bytes<O>(character, bytes, current_index, end_index, error_code);
    buff += character;
  }

  value = std::move(buff);
  return true;
}

} // namespace detail

} // namespace alpaca
#endif
