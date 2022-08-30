#pragma once
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/type_info.h>
#include <set>
#include <system_error>
#include <unordered_set>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size()>
typename std::enable_if<std::is_aggregate_v<T> && !is_array_type<T>::value,
                        void>::type
type_info(
    std::vector<uint8_t> &typeids,
    std::unordered_map<std::string_view, std::size_t> &struct_visitor_map);

template <typename T>
typename std::enable_if<is_specialization<T, std::set>::value, void>::type
type_info(
    std::vector<uint8_t> &typeids,
    std::unordered_map<std::string_view, std::size_t> &struct_visitor_map) {
  typeids.push_back(to_byte<field_type::set>());
  using value_type = typename T::value_type;
  type_info<value_type>(typeids, struct_visitor_map);
}

template <typename T>
typename std::enable_if<is_specialization<T, std::unordered_set>::value,
                        void>::type
type_info(
    std::vector<uint8_t> &typeids,
    std::unordered_map<std::string_view, std::size_t> &struct_visitor_map) {
  typeids.push_back(to_byte<field_type::unordered_set>());
  using value_type = typename T::value_type;
  type_info<value_type>(typeids, struct_visitor_map);
}

template <options O, typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <options O, typename T>
void to_bytes_from_set_type(const T &input, std::vector<uint8_t> &bytes) {
  // save set size
  to_bytes_router<O>(input.size(), bytes);

  // save values in set
  for (const auto &value : input) {
    using decayed_key_type = typename std::decay<typename T::value_type>::type;
    to_bytes_router<O, decayed_key_type>(value, bytes);
  }
}

template <options O, typename T, typename U>
void to_bytes(T &bytes, const std::set<U> &input) {
  to_bytes_from_set_type<O>(input, bytes);
}

template <options O, typename T, typename U>
void to_bytes(T &bytes, const std::unordered_set<U> &input) {
  to_bytes_from_set_type<O>(input, bytes);
}

template <options O, typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::size_t &end_index,
                       std::error_code &error_code);

template <options O, typename T>
void from_bytes_to_set(T &set, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index, std::size_t &end_index,
                       std::error_code &error_code) {
  // current byte is the size of the set
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  if (size > end_index - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return;
  }

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    typename T::value_type value{};
    from_bytes_router<O>(value, bytes, current_index, end_index, error_code);
    set.insert(value);
  }
}

template <options O, typename T>
bool from_bytes(std::set<T> &output, const std::vector<uint8_t> &bytes,
                std::size_t &byte_index, std::size_t &end_index,
                std::error_code &error_code) {
  from_bytes_to_set<O>(output, bytes, byte_index, end_index, error_code);
  return true;
}

template <options O, typename T>
bool from_bytes(std::unordered_set<T> &output,
                const std::vector<uint8_t> &bytes, std::size_t &byte_index,
                std::size_t &end_index, std::error_code &error_code) {
  from_bytes_to_set<O>(output, bytes, byte_index, end_index, error_code);
  return true;
}

} // namespace detail

} // namespace alpaca