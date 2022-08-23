#pragma once
#include <serialize/detail/aggregate_arity.h>
#include <serialize/detail/from_bytes.h>
#include <serialize/detail/print_bytes.h>
#include <serialize/detail/struct_nth_field.h>
#include <serialize/detail/to_bytes.h>
#include <serialize/detail/type_traits.h>
#include <serialize/detail/variable_length_encoding.h>

template <typename T, std::size_t index>
void serialize(const T &s, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_pair_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_tuple_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_list_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes) {
  // unsigned or signed integer types
  // char, bool
  // float, double
  if constexpr (std::is_arithmetic_v<T>) {
    // use variable-length encoding if possible
    detail::to_bytes(input, bytes);
  }
  // pair
  else if constexpr (detail::is_pair<T>::value) {
    to_bytes_from_pair_type<T>(input, bytes);
  }
  // std::string
  else if constexpr (detail::is_string::detect<T>) {
    detail::to_bytes(input, bytes);
  }
  // tuple
  else if constexpr (detail::is_tuple<T>::value) {
    to_bytes_from_tuple_type<T>(input, bytes);
  }
  // vector
  else if constexpr (detail::is_vector<T>::value) {
    to_bytes_from_list_type<T>(input, bytes);
  }
  // nested struct
  else if constexpr (std::is_class_v<T>) {
    serialize<T, 0>(input, bytes);
  } else {
    /// TODO: throw error unsupported type
    detail::append(input, bytes);
  }
}

// Specialization for pair

template <typename T>
void save_pair_value(const T &pair, std::vector<uint8_t> &bytes) {
  detail::to_bytes(pair.first, bytes);
  detail::to_bytes(pair.second, bytes);
}

template <typename T>
void to_bytes_from_pair_type(const T &input, std::vector<uint8_t> &bytes) {
  // value of each element
  save_pair_value<T>(input, bytes);
}

// Specialization for tuple

template <typename T, std::size_t index>
void save_tuple_value(const T &tuple, std::vector<uint8_t> &bytes) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    detail::to_bytes(std::get<index>(tuple), bytes);
    save_tuple_value<T, index + 1>(tuple, bytes);
  }
}

template <typename T>
void to_bytes_from_tuple_type(const T &input, std::vector<uint8_t> &bytes) {
  // value of each element
  save_tuple_value<T, 0>(input, bytes);
}

// Specialization for vector

template <typename T>
void to_bytes_from_list_type(const T &input, std::vector<uint8_t> &bytes) {
  // save vector size
  detail::to_bytes(input.size(), bytes);

  // value of each element in list
  for (const auto &v : input) {
    // check if the value_type is a nested list type
    using decayed_value_type = typename std::decay<decltype(v)>::type;
    to_bytes_router<decayed_value_type>(v, bytes);
  }
}

template <typename T, std::size_t index>
void serialize(const T &s, std::vector<uint8_t> &bytes) {
  constexpr static auto max_index =
      detail::aggregate_arity<std::remove_cv_t<T>>::size();
  if constexpr (index < max_index) {
    decltype(auto) field = detail::get<index>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // serialize field
    to_bytes_router<decayed_field_type>(field, bytes);

    // go to next field
    serialize<T, index + 1>(s, bytes);
  }
}

template <typename T> std::vector<uint8_t> serialize(T &s) {
  std::vector<uint8_t> bytes{};
  serialize<T, 0>(s, bytes);
  return bytes;
}

template <typename T, std::size_t index = 0>
void deserialize(T &s, const std::vector<uint8_t> &bytes,
                 std::size_t byte_index = 0) {
  constexpr static auto max_index =
      detail::aggregate_arity<std::remove_cv_t<T>>::size();
  if constexpr (index < max_index) {
    decltype(auto) field = detail::get<index>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // set value for current field
    /// TODO: Check result of from_bytes call and proceed accordingly
    if constexpr (detail::is_string::detect<decayed_field_type>) {
      detail::from_bytes_to_string(field, bytes, byte_index);
    } else if constexpr (detail::is_vector<decayed_field_type>::value) {
      detail::from_bytes_to_vector(field, bytes, byte_index);
    } else {
      detail::from_bytes<decayed_field_type>(field, bytes, byte_index);
    }

    // go to next field
    deserialize<T, index + 1>(s, bytes, byte_index);
  }
}

template <typename T> T deserialize(const std::vector<uint8_t> &bytes) {
  T object{};
  deserialize<T, 0>(object, bytes);
  return object;
}