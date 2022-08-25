#pragma once
#include <alpaca/detail/aggregate_arity.h>
#include <alpaca/detail/endian.h>
#include <alpaca/detail/from_bytes.h>
#include <alpaca/detail/print_bytes.h>
#include <alpaca/detail/struct_nth_field.h>
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/type_traits.h>
#include <alpaca/detail/variable_length_encoding.h>

namespace alpaca {

// Forward declares
template <typename T, std::size_t index>
void serialize(const T &s, std::vector<uint8_t> &bytes);

namespace detail {

// Start of serialization functions

template <typename T>
void to_bytes_from_array_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_pair_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_map_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_set_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_tuple_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_list_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes) {
  // unique_ptr
  if constexpr (detail::is_specialization<T, std::unique_ptr>::value) {
    auto has_value = false;
    if (input) {
      has_value = true;
    }

    // save if ptr has value
    to_bytes_router<bool>(has_value, bytes);

    // save value
    if (has_value) {
      using element_type = typename T::element_type;
      to_bytes_router<element_type>(*input, bytes);
    }
  }
  // unsigned or signed integer types
  // char, bool
  // float, double
  else if constexpr (std::is_arithmetic_v<T>) {
    // use variable-length encoding if possible
    detail::to_bytes(input, bytes);
  }
  // array
  else if constexpr (detail::is_array<T>::value) {
    to_bytes_from_array_type<T>(input, bytes);
  }
  // enum class
  else if constexpr (std::is_enum<T>::value) {
    using underlying_type = typename std::underlying_type<T>::type;
    to_bytes_router<underlying_type>(static_cast<underlying_type>(input),
                                     bytes);
  }
  // optional
  else if constexpr (detail::is_specialization<T, std::optional>::value) {
    const auto has_value = input.has_value();

    // save if optional has value
    to_bytes_router<bool>(has_value, bytes);

    // save value
    if (has_value) {
      using value_type = typename T::value_type;
      to_bytes_router<value_type>(input.value(), bytes);
    }
  }
  // pair
  else if constexpr (detail::is_pair<T>::value) {
    to_bytes_from_pair_type<T>(input, bytes);
  }
  // std::string
  else if constexpr (detail::is_string::detect<T>) {
    detail::to_bytes(input, bytes);
  }
  // map-like
  else if constexpr (detail::is_mappish<T>::value) {
    to_bytes_from_map_type<T>(input, bytes);
  }
  // set, unordered_set
  else if constexpr (detail::is_specialization<T, std::set>::value ||
                     detail::is_specialization<T, std::unordered_set>::value) {
    to_bytes_from_set_type<T>(input, bytes);
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
    throw std::invalid_argument("unsupported type");
  }
}

// Specialization for array

template <typename T>
void to_bytes_from_array_type(const T &input, std::vector<uint8_t> &bytes) {
  using decayed_value_type = typename std::decay<typename T::value_type>::type;

  // value of each element in list
  for (const auto &v : input) {
    to_bytes_router<decayed_value_type>(v, bytes);
  }
}

// Specialization for pair

template <typename T>
void save_pair_value(const T &pair, std::vector<uint8_t> &bytes) {
  to_bytes_router(pair.first, bytes);
  to_bytes_router(pair.second, bytes);
}

template <typename T>
void to_bytes_from_pair_type(const T &input, std::vector<uint8_t> &bytes) {
  // value of each element
  save_pair_value<T>(input, bytes);
}

// Specialization for map

template <typename T>
void to_bytes_from_map_type(const T &input, std::vector<uint8_t> &bytes) {
  // save map size
  detail::to_bytes(input.size(), bytes);

  // save key,value pairs in map
  for (const auto &[key, value] : input) {

    using decayed_key_type = typename std::decay<decltype(key)>::type;
    to_bytes_router<decayed_key_type>(key, bytes);

    using decayed_value_type = typename std::decay<decltype(value)>::type;
    to_bytes_router<decayed_value_type>(value, bytes);
  }
}

// Specialization for set/unordered_set

template <typename T>
void to_bytes_from_set_type(const T &input, std::vector<uint8_t> &bytes) {
  // save set size
  detail::to_bytes(input.size(), bytes);

  // save values in set
  for (const auto &value : input) {
    using decayed_key_type = typename std::decay<typename T::value_type>::type;
    to_bytes_router<decayed_key_type>(value, bytes);
  }
}

// Specialization for tuple

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

} // namespace detail

template <typename T, std::size_t index>
void serialize(const T &s, std::vector<uint8_t> &bytes) {
  constexpr static auto max_index =
      detail::aggregate_arity<std::remove_cv_t<T>>::size();
  if constexpr (index < max_index) {
    const auto& ref = s;
    decltype(auto) field = detail::get<index, decltype(ref)>(ref);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // serialize field
    detail::to_bytes_router<decayed_field_type>(field, bytes);

    // go to next field
    serialize<T, index + 1>(s, bytes);
  }
}

template <typename T> std::vector<uint8_t> serialize(const T &s) {
  std::vector<uint8_t> bytes{};
  serialize<T, 0>(s, bytes);
  return bytes;
}

// Overload of serialize where the number of fields in struct is SPECIFIED

/// N -> number of fields in struct
/// I -> field to start from
template <typename T, std::size_t N, std::size_t I>
void serialize(const T &s, std::vector<uint8_t> &bytes) {
  if constexpr (I < N) {
    const auto& ref = s;
    decltype(auto) field = detail::get<I, decltype(ref), N>(ref);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // serialize field
    detail::to_bytes_router<decayed_field_type>(field, bytes);

    // go to next field
    serialize<T, N, I + 1>(s, bytes);
  }
}

template <typename T, std::size_t N, std::size_t I = 0>
std::vector<uint8_t> serialize(const T &s) {
  std::vector<uint8_t> bytes{};
  serialize<T, N, I>(s, bytes);
  return bytes;
}

// Forward declares
template <typename T, std::size_t index>
void deserialize(T &s, const std::vector<uint8_t> &bytes,
                 std::size_t byte_index);

namespace detail {

// Start of deserialization functions

template <typename T>
bool from_bytes_to_array(T &value, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index);

template <typename T>
bool from_bytes_to_pair(T &pair, const std::vector<uint8_t> &bytes,
                        std::size_t &current_index);

template <typename T>
bool from_bytes_to_map(T &map, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index);

template <typename T>
bool from_bytes_to_set(T &set, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index);

template <typename T>
bool from_bytes_to_tuple(T &tuple, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index);

template <typename T>
bool from_bytes_to_vector(std::vector<T> &value,
                          const std::vector<uint8_t> &bytes,
                          std::size_t &current_index);

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index) {
  // unique_ptr
  if constexpr (detail::is_specialization<T, std::unique_ptr>::value) {
    // current byte is the `has_value` byte
    bool has_value = detail::decode_varint<bool>(bytes, byte_index);

    if (has_value) {
      // read value of unique_ptr
      using element_type = typename T::element_type;
      element_type value;
      from_bytes_router(value, bytes, byte_index);
      output = std::unique_ptr<element_type>(new element_type{value});
    }
    else {
      output = nullptr;
    }
  }
  // unsigned or signed integer types
  // char, bool
  // float, double
  else if constexpr (std::is_arithmetic_v<T>) {
    detail::from_bytes(output, bytes, byte_index);
  }
  // array
  else if constexpr (detail::is_array<T>::value) {
    from_bytes_to_array(output, bytes, byte_index);
  }
  // enum class
  else if constexpr (std::is_enum<T>::value) {
    using underlying_type = typename std::underlying_type<T>::type;
    underlying_type underlying_value;
    from_bytes_router<underlying_type>(underlying_value, bytes, byte_index);
    output = static_cast<T>(underlying_value);
    byte_index += 1;
  }
  // pair
  else if constexpr (detail::is_pair<T>::value) {
    from_bytes_to_pair(output, bytes, byte_index);
  }
  // optional
  else if constexpr (detail::is_specialization<T, std::optional>::value) {
    // current byte is the `has_value` byte
    bool has_value = detail::decode_varint<bool>(bytes, byte_index);

    if (has_value) {
      // read value of optional
      using value_type = typename T::value_type;
      value_type value;
      from_bytes_router(value, bytes, byte_index);
      output = value;
    }
  }
  // std::string
  else if constexpr (detail::is_string::detect<T>) {
    detail::from_bytes(output, bytes, byte_index);
  }
  // map-like
  else if constexpr (detail::is_mappish<T>::value) {
    from_bytes_to_map(output, bytes, byte_index);
  }
  // set, unordered_set
  else if constexpr (detail::is_specialization<T, std::set>::value ||
                     detail::is_specialization<T, std::unordered_set>::value) {
    from_bytes_to_set(output, bytes, byte_index);
  }
  // tuple
  else if constexpr (detail::is_tuple<T>::value) {
    from_bytes_to_tuple(output, bytes, byte_index);
  }
  // vector
  else if constexpr (detail::is_vector<T>::value) {
    from_bytes_to_vector(output, bytes, byte_index);
  }
  // nested struct
  else if constexpr (std::is_class_v<T>) {
    deserialize<T, 0>(output, bytes, byte_index);
    byte_index++;
  } else {
    throw std::invalid_argument("unsupported type");
  }
}

// Specialization for array

template <typename T>
bool from_bytes_to_array(T &value, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index) {

  using decayed_value_type = typename std::decay<typename T::value_type>::type;

  constexpr auto size = std::tuple_size<T>::value;

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    decayed_value_type v{};
    from_bytes_router(v, bytes, current_index);
    value[i] = v;
  }

  return true;
}

// Specialization for pair

template <typename T>
void load_pair_value(T &pair, const std::vector<uint8_t> &bytes,
                     std::size_t &current_index) {
  from_bytes_router(pair.first, bytes, current_index);
  from_bytes_router(pair.second, bytes, current_index);
}

template <typename T>
bool from_bytes_to_pair(T &pair, const std::vector<uint8_t> &bytes,
                        std::size_t &current_index) {
  load_pair_value<T>(pair, bytes, current_index);
  return true;
}

// Specialization for map

template <typename T>
bool from_bytes_to_map(T &map, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index) {
  // current byte is the size of the map
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    typename T::key_type key{};
    from_bytes_router(key, bytes, current_index);

    typename T::mapped_type value{};
    from_bytes_router(value, bytes, current_index);

    map.insert(std::make_pair(key, value));
  }

  return true;
}

// Specialization for set

template <typename T>
bool from_bytes_to_set(T &set, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index) {
  // current byte is the size of the set
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    typename T::value_type value{};
    from_bytes_router(value, bytes, current_index);
    set.insert(value);
  }

  return true;
}

// Specialization for tuple

template <typename T, std::size_t index>
void load_tuple_value(T &tuple, const std::vector<uint8_t> &bytes,
                      std::size_t &current_index) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    from_bytes_router(std::get<index>(tuple), bytes, current_index);
    load_tuple_value<T, index + 1>(tuple, bytes, current_index);
  }
}

template <typename T>
bool from_bytes_to_tuple(T &tuple, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index) {
  load_tuple_value<T, 0>(tuple, bytes, current_index);
  return true;
}

// Specialization for vector

template <typename T>
bool from_bytes_to_vector(std::vector<T> &value,
                          const std::vector<uint8_t> &bytes,
                          std::size_t &current_index) {

  // current byte is the size of the vector
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    T v{};
    from_bytes_router(v, bytes, current_index);
    value.push_back(v);
  }

  return true;
}

} // namespace detail

template <typename T, std::size_t index>
void deserialize(T &s, const std::vector<uint8_t> &bytes,
                 std::size_t byte_index) {
  constexpr static auto max_index =
      detail::aggregate_arity<std::remove_cv_t<T>>::size();
  if constexpr (index < max_index) {
    decltype(auto) field = detail::get<index>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // load current field
    detail::from_bytes_router<decayed_field_type>(field, bytes, byte_index);

    // go to next field
    deserialize<T, index + 1>(s, bytes, byte_index);
  }
}

template <typename T> T deserialize(const std::vector<uint8_t> &bytes) {
  T object{};
  deserialize<T, 0>(object, bytes, 0);
  return object;
}

// Overload of deserialize where the number of struct fields is SPECIFIED

/// N -> number of fields in struct
/// I -> field to start from
template <typename T, std::size_t N, std::size_t I>
void deserialize(T &s, const std::vector<uint8_t> &bytes,
                 std::size_t byte_index) {
  if constexpr (I < N) {
    decltype(auto) field = detail::get<I, T, N>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // load current field
    detail::from_bytes_router<decayed_field_type>(field, bytes, byte_index);

    // go to next field
    deserialize<T, N, I + 1>(s, bytes, byte_index);
  }
}

template <typename T, std::size_t N, std::size_t I = 0>
T deserialize(const std::vector<uint8_t> &bytes) {
  T object{};
  deserialize<T, N, I>(object, bytes, 0);
  return object;
}

} // namespace alpaca