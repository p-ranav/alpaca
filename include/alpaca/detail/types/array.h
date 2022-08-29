#pragma once
#include <alpaca/detail/type_info.h>
#include <array>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
typename std::enable_if<is_array_type<T>::value, void>::type type_info(
    std::vector<uint8_t> &typeids,
    std::unordered_map<std::string_view, std::size_t> &struct_visitor_map) {
  typeids.push_back(to_byte<field_type::array>());
  typeids.push_back(std::tuple_size_v<T>);
  using value_type = typename T::value_type;
  type_info<value_type>(typeids, struct_visitor_map);
}

template <options O, typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <options O, typename T>
void to_bytes_from_array_type(const T &input, std::vector<uint8_t> &bytes) {
  using decayed_value_type = typename std::decay<typename T::value_type>::type;

  // value of each element in list
  for (const auto &v : input) {
    to_bytes_router<O, decayed_value_type>(v, bytes);
  }
}

template <options O, typename T, typename U, std::size_t N>
void to_bytes(T &bytes, const std::array<U, N> &input) {
  to_bytes_from_array_type<O>(input, bytes);
}

template <options O, typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <options O, typename T>
void from_bytes_to_array(T &value, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index,
                         std::error_code &error_code) {

  using decayed_value_type = typename std::decay<typename T::value_type>::type;

  constexpr auto size = std::tuple_size<T>::value;

  if (size > bytes.size() - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return;
  }

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    decayed_value_type v{};
    from_bytes_router<O>(v, bytes, current_index, error_code);
    value[i] = v;
  }
}

template <options O, typename U, std::size_t N>
bool from_bytes(std::array<U, N> &output, const std::vector<uint8_t> &bytes,
                std::size_t &byte_index, std::error_code &error_code) {
  from_bytes_to_array<O>(output, bytes, byte_index, error_code);
  return true;
}

} // namespace detail

} // namespace alpaca