#pragma once
#include <alpaca/detail/type_info.h>
#include <array>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
struct is_array_type: std::false_type {};

template <class T, std::size_t N>
struct is_array_type<std::array<T, N>> : std::true_type {};

template <typename T, 
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size()>
typename std::enable_if<std::is_aggregate_v<T> && !is_array_type<T>::value, void>::type
type_info(std::vector<uint8_t>& typeids, 
  std::unordered_map<std::string_view, std::size_t>& struct_visitor_map);

template <typename T>
typename std::enable_if<is_array_type<T>::value, void>::type
type_info(std::vector<uint8_t>& typeids, 
  std::unordered_map<std::string_view, std::size_t>& struct_visitor_map) {
  typeids.push_back(to_byte<field_type::array>());
  typeids.push_back(std::tuple_size_v<T>);
  using value_type = typename T::value_type;
  type_info<value_type>(typeids, struct_visitor_map);
}

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_array_type(const T &input, std::vector<uint8_t> &bytes) {
  using decayed_value_type = typename std::decay<typename T::value_type>::type;

  // value of each element in list
  for (const auto &v : input) {
    to_bytes_router<decayed_value_type>(v, bytes);
  }
}

template <typename T, typename U, std::size_t N>
void to_bytes(T &bytes, const std::array<U, N> &input) {
  to_bytes_from_array_type(input, bytes);
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T>
void from_bytes_to_array(T &value, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index,
                         std::error_code &error_code) {

  using decayed_value_type = typename std::decay<typename T::value_type>::type;

  constexpr auto size = std::tuple_size<T>::value;

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    decayed_value_type v{};
    from_bytes_router(v, bytes, current_index, error_code);
    value[i] = v;
  }
}

template <typename U, std::size_t N>
bool from_bytes(std::array<U, N> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
    from_bytes_to_array(output, bytes, byte_index, error_code);
    return true;
}

}

}