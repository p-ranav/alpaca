#pragma once
#include <array>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

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