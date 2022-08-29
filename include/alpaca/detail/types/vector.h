#pragma once
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/type_info.h>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
typename std::enable_if<is_specialization<T, std::vector>::value, void>::type
type_info(
    std::vector<uint8_t> &typeids,
    std::unordered_map<std::string_view, std::size_t> &struct_visitor_map) {
  typeids.push_back(to_byte<field_type::vector>());
  using value_type = typename T::value_type;
  type_info<value_type>(typeids, struct_visitor_map);
}

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_vector_type(const T &input, std::vector<uint8_t> &bytes) {
  // save vector size
  to_bytes_router<std::size_t>(input.size(), bytes);

  // value of each element in list
  for (const auto &v : input) {
    // check if the value_type is a nested list type
    using decayed_value_type = typename std::decay<decltype(v)>::type;
    to_bytes_router<decayed_value_type>(v, bytes);
  }
}

template <typename T, typename U>
void to_bytes(T &bytes, const std::vector<U> &input) {
  to_bytes_from_vector_type(input, bytes);
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T>
void from_bytes_to_vector(std::vector<T> &value,
                          const std::vector<uint8_t> &bytes,
                          std::size_t &current_index,
                          std::error_code &error_code) {

  // current byte is the size of the vector
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  if (size > bytes.size() - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return;
  }

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    T v{};
    from_bytes_router(v, bytes, current_index, error_code);
    if (error_code) {
      // something went wrong
      return;
    }
    value.push_back(v);
  }
}

template <typename T>
bool from_bytes(std::vector<T> &output, const std::vector<uint8_t> &bytes,
                std::size_t &byte_index, std::error_code &error_code) {
  from_bytes_to_vector(output, bytes, byte_index, error_code);
  return true;
}

} // namespace detail

} // namespace alpaca