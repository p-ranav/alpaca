#pragma once
#include <system_error>
#include <tuple>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T, std::size_t index>
void save_tuple_value(const T &tuple, std::vector<uint8_t> &bytes) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    to_bytes_router(std::get<index>(tuple), bytes);
    save_tuple_value<T, index + 1>(tuple, bytes);
  }
}

template <typename T>
void to_bytes_from_tuple_type(const T &input, std::vector<uint8_t> &bytes) {
  // value of each element
  save_tuple_value<T, 0>(input, bytes);
}

template <typename T, typename... U>
void append(T &bytes, const std::tuple<U...> &input) {
    to_bytes_from_tuple_type(input, bytes);
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T, std::size_t index>
void load_tuple_value(T &tuple, const std::vector<uint8_t> &bytes,
                      std::size_t &current_index, std::error_code &error_code) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    from_bytes_router(std::get<index>(tuple), bytes, current_index, error_code);
    load_tuple_value<T, index + 1>(tuple, bytes, current_index, error_code);
  }
}

template <typename T>
void from_bytes_to_tuple(T &tuple, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index,
                         std::error_code &error_code) {
  load_tuple_value<T, 0>(tuple, bytes, current_index, error_code);
}

template <typename... T>
bool read_bytes(std::tuple<T...> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
    from_bytes_to_tuple(output, bytes, byte_index, error_code);
    return true;
}

}

}