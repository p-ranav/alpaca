#pragma once
#include <alpaca/detail/type_info.h>
#include <optional>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T, 
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size()>
typename std::enable_if<std::is_aggregate_v<T>, void>::type
type_info(std::vector<uint8_t>& typeids, 
  std::unordered_map<std::string_view, std::size_t>& struct_visitor_map);

template <typename T>
typename std::enable_if<is_specialization<T, std::optional>::value, void>::type
type_info(std::vector<uint8_t>& typeids, 
  std::unordered_map<std::string_view, std::size_t>& struct_visitor_map) {
  typeids.push_back(to_byte<field_type::optional>());
  using value_type = typename T::value_type;
  type_info<value_type>(typeids, struct_visitor_map);
}

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T, typename U>
void to_bytes(T &bytes, const std::optional<U> &input) {
    const auto has_value = input.has_value();

    // save if optional has value
    to_bytes_router<bool>(has_value, bytes);

    // save value
    if (has_value) {
        to_bytes_router<U>(input.value(), bytes);
    }
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T>
bool from_bytes(std::optional<T> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {

    // current byte is the `has_value` byte
    bool has_value = static_cast<bool>(bytes[byte_index++]);

    if (has_value) {
      // read value of optional
      T value;
      from_bytes_router(value, bytes, byte_index, error_code);
      output = value;
    }

    return true;
}

}

}