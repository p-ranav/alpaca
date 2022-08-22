#pragma once
#include <cstring>
#include <iterator>
#include <serialize/detail/type_traits.h>
#include <serialize/detail/variable_length_encoding.h>

namespace detail {

template <typename T, typename U> void append(const T &value, U &bytes) {
  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

// Integral types
template <typename T>
typename std::enable_if<std::is_integral_v<T>, void>::type
to_bytes(T input, std::vector<uint8_t> &bytes) {
  encode_varint<T>(input, bytes);
}

// Specializations for particular numeric types

static inline void to_bytes(bool input, std::vector<uint8_t> &bytes) {
  append(input, bytes);
}

static inline void to_bytes(char input, std::vector<uint8_t> &bytes) {
  to_bytes(static_cast<uint8_t>(input), bytes);
}

static inline void to_bytes(const float input, std::vector<uint8_t> &bytes) {
  append(input, bytes);
}

static inline void to_bytes(const double input, std::vector<uint8_t> &bytes) {
  append(input, bytes);
}

// Specialization for string

static inline void to_bytes(const std::string &input,
                            std::vector<uint8_t> &bytes) {
  // save string length
  to_bytes(input.size(), bytes);

  for (auto &c : input) {
    append(c, bytes);
  }
}

// Specialization for tuple

template <typename T, std::size_t index>
void save_tuple_value(const T &tuple, std::vector<uint8_t> &bytes) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    to_bytes(std::get<index>(tuple), bytes);
    save_tuple_value<T, index + 1>(tuple, bytes);
  }
}

template <typename T>
void to_bytes_from_tuple_type(const T &input, std::vector<uint8_t> &bytes) {
  // value of each element
  save_tuple_value<T, 0>(input, bytes);
}

// Specialization for pair

template <typename T>
void save_pair_value(const T &pair, std::vector<uint8_t> &bytes) {
  to_bytes(pair.first, bytes);
  to_bytes(pair.second, bytes);
}

template <typename T>
void to_bytes_from_pair_type(const T &input, std::vector<uint8_t> &bytes) {
  // value of each element
  save_pair_value<T>(input, bytes);
}

// Specialization for vector

template <typename T>
void to_bytes_from_list_type(const T &input, std::vector<uint8_t> &bytes) {
  // save vector size
  to_bytes(input.size(), bytes);

  // value of each element in list
  for (const auto &v : input) {
    // check if the value_type is a nested list type
    using decayed_value_type = typename std::decay<decltype(v)>::type;
    if constexpr (is_vector<decayed_value_type>::value) {
      to_bytes_from_list_type<decayed_value_type>(v, bytes);
    } else if constexpr (is_tuple<decayed_value_type>::value) {
      to_bytes_from_tuple_type<decayed_value_type>(v, bytes);
    } else if constexpr (is_string::detect<decayed_value_type>) {
      to_bytes(v, bytes);
    } else if constexpr (std::is_integral_v<decayed_value_type>) {
      // use variable-length encoding if possible
      to_bytes(v, bytes);
    } else {
      // dump all the values
      // note: no attempted compression for integer types
      append(v, bytes);
    }
  }
}

} // namespace detail