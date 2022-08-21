#pragma once
#include <serialize/detail/append.h>
#include <serialize/detail/is_pair.h>
#include <serialize/detail/is_string.h>
#include <serialize/detail/is_tuple.h>
#include <serialize/detail/is_vector.h>
#include <serialize/detail/type.h>
#include <type_traits>

namespace detail {

// Forward declarations
template <bool save_type_info, bool attempt_compression>
void to_bytes(uint64_t input, std::vector<uint8_t> &bytes);

template <typename T, std::size_t index>
void save_tuple_value_type(std::vector<uint8_t> &bytes);

template <typename T> void save_pair_value_type(std::vector<uint8_t> &bytes);

// append the type of value to bytes
template <typename T, typename U> void append_value_type(U &bytes) {
  if constexpr (std::is_same<T, bool>::value) {
    append(type::boolean, bytes);
  } else if constexpr (std::is_same<T, char>::value) {
    append(type::uint8, bytes);
  } else if constexpr (std::is_same<T, uint8_t>::value) {
    append(type::uint8, bytes);
  } else if constexpr (std::is_same<T, uint16_t>::value) {
    append(type::uint16, bytes);
  } else if constexpr (std::is_same<T, uint32_t>::value) {
    append(type::uint32, bytes);
  } else if constexpr (std::is_same<T, uint64_t>::value) {
    append(type::uint64, bytes);
  } else if constexpr (std::is_same<T, int8_t>::value) {
    append(type::int8, bytes);
  } else if constexpr (std::is_same<T, int16_t>::value) {
    append(type::int16, bytes);
  } else if constexpr (std::is_same<T, int32_t>::value) {
    append(type::int32, bytes);
  } else if constexpr (std::is_same<T, int64_t>::value) {
    append(type::int64, bytes);
  } else if constexpr (std::is_same<T, float>::value) {
    append(type::float32, bytes);
  } else if constexpr (std::is_same<T, double>::value) {
    append(type::float64, bytes);
  } else if constexpr (is_string::detect<T>) {
    append(type::string, bytes);
  } else if constexpr (is_vector<T>::value) {
    append(type::vector, bytes);
    append_value_type<typename std::decay<typename T::value_type>::type>(bytes);
  } else if constexpr (is_tuple<T>::value) {
    append(type::tuple, bytes);
    // save number of tuple elements
    to_bytes<true, true>(std::tuple_size<T>::value, bytes);
    // save type of each tuple element
    save_tuple_value_type<T, 0>(bytes);
  } else if constexpr (is_pair<T>::value) {
    append(type::pair, bytes);
    // save type of each pair element
    save_pair_value_type<T>(bytes);
  } else if constexpr (std::is_class<T>::value) {
    append(type::struct_, bytes);
  }
}

} // namespace detail