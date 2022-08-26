#pragma once
#include <alpaca/detail/aggregate_arity.h>
#include <alpaca/detail/crc32.h>
#include <alpaca/detail/endian.h>
#include <alpaca/detail/from_bytes.h>
#include <alpaca/detail/print_bytes.h>
#include <alpaca/detail/struct_nth_field.h>
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/type_traits.h>
#include <alpaca/detail/variable_length_encoding.h>
#include <alpaca/detail/variant_nth_field.h>
#include <alpaca/types/tuple.h>
#include <system_error>

namespace alpaca {

// Forward declares
template <typename T, std::size_t N, std::size_t I>
void serialize(const T &s, std::vector<uint8_t> &bytes);

namespace detail {

// Start of serialization functions

template <typename T>
void to_bytes_from_array_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_set_type(const T &input, std::vector<uint8_t> &bytes);

template <typename T>
void to_bytes_from_list_type(const T &input, std::vector<uint8_t> &bytes);

// version for nested struct/class types
// incidentally, also works for std::pair
template <typename T, typename U>
typename std::enable_if<std::is_aggregate_v<U>, void>::type 
append(T &bytes, const U &input) {
  serialize<U, detail::aggregate_arity<std::remove_cv_t<U>>::size(), 0>(
      input, bytes);
}

template <typename T, typename U>
typename std::enable_if<!std::is_aggregate_v<U> && std::is_class_v<U>, void>::type 
append(T &bytes, const U &input) {
  serialize<U, detail::aggregate_arity<std::remove_cv_t<U>>::size(), 0>(
      input, bytes);
}

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
  else if constexpr (std::is_array_v<T>) {
    to_bytes_from_array_type<T>(input, bytes);
  }
  // enum class
  else if constexpr (std::is_enum<T>::value) {
    using underlying_type = typename std::underlying_type<T>::type;
    to_bytes_router<underlying_type>(static_cast<underlying_type>(input),
                                     bytes);
  }
  // std::string
  else if constexpr (detail::is_string::detect<T>) {
    detail::to_bytes(input, bytes);
  }
  // set, unordered_set
  else if constexpr (detail::is_specialization<T, std::set>::value ||
                     detail::is_specialization<T, std::unordered_set>::value) {
    to_bytes_from_set_type<T>(input, bytes);
  }
  // variant
  else if constexpr (detail::is_specialization<T, std::variant>::value) {
    std::size_t index = input.index();

    // save index of variant
    to_bytes_router<std::size_t>(index, bytes);

    // save value of variant
    const auto visitor = [&bytes](auto &&arg) { to_bytes_router(arg, bytes); };
    std::visit(visitor, input);
  }
  // vector
  else if constexpr (detail::is_vector<T>::value) {
    to_bytes_from_list_type<T>(input, bytes);
  }
  // everything else
  else {
    detail::append(bytes, input);
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

/// N -> number of fields in struct
/// I -> field to start from
template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
void serialize(const T &s, std::vector<uint8_t> &bytes) {
  if constexpr (I < N) {
    const auto &ref = s;
    decltype(auto) field = detail::get<I, decltype(ref), N>(ref);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // serialize field
    detail::to_bytes_router<decayed_field_type>(field, bytes);

    // go to next field
    serialize<T, N, I + 1>(s, bytes);
  }
}

template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
std::vector<uint8_t> serialize(const T &s) {
  std::vector<uint8_t> bytes{};
  serialize<T, N, I>(s, bytes);
  return bytes;
}

// Overloads that append CRC32 of bytes

template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
void serialize(const T &s, std::vector<uint8_t> &bytes, bool generate_crc) {
  serialize<T, N, I>(s, bytes);

  if (N > 0 && generate_crc) {
    // calculate crc32 for byte array and
    // append uint32_t to the end
    uint32_t crc = crc32_fast(bytes.data(), bytes.size());
    detail::append(bytes, crc);
  }
}

template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
std::vector<uint8_t> serialize(const T &s, bool generate_crc) {
  std::vector<uint8_t> bytes{};
  serialize<T, N, I>(s, bytes, generate_crc);
  return bytes;
}

// Forward declares
template <typename T, std::size_t N, std::size_t index>
void deserialize(T &s, const std::vector<uint8_t> &bytes,
                 std::size_t &byte_index, std::error_code &error_code);

namespace detail {

// Start of deserialization functions

template <typename T>
void from_bytes_to_array(T &value, const std::vector<uint8_t> &bytes,
                         std::size_t &current_index,
                         std::error_code &error_code);

template <typename T>
void from_bytes_to_set(T &set, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index, std::error_code &error_code);

template <typename T>
void from_bytes_to_vector(std::vector<T> &value,
                          const std::vector<uint8_t> &bytes,
                          std::size_t &current_index,
                          std::error_code &error_code);

// version for nested struct/class types
template <typename T>
typename std::enable_if<std::is_aggregate_v<T>, bool>::type 
read_bytes(T &value, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
  deserialize<T, detail::aggregate_arity<std::remove_cv_t<T>>::size(), 0>(
      value, bytes, byte_index, error_code);
  return true;
}

template <typename T>
typename std::enable_if<!std::is_aggregate_v<T> && std::is_class_v<T>, bool>::type 
read_bytes(T &value, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
  deserialize<T, detail::aggregate_arity<std::remove_cv_t<T>>::size(), 0>(
      value, bytes, byte_index, error_code);
  return true;
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code) {
  // unique_ptr
  if constexpr (detail::is_specialization<T, std::unique_ptr>::value) {
    // current byte is the `has_value` byte
    bool has_value = false;
    detail::read_bytes<bool>(has_value, bytes, byte_index, error_code);

    if (has_value) {
      // read value of unique_ptr
      using element_type = typename T::element_type;
      element_type value;
      from_bytes_router(value, bytes, byte_index, error_code);
      output = std::unique_ptr<element_type>(new element_type{value});
    } else {
      output = nullptr;
    }
  }
  // unsigned or signed integer types
  // char, bool
  // float, double
  else if constexpr (std::is_arithmetic_v<T>) {
    detail::from_bytes(output, bytes, byte_index, error_code);
  }
  // array
  else if constexpr (std::is_array_v<T>) {
    from_bytes_to_array(output, bytes, byte_index, error_code);
  }
  // enum class
  else if constexpr (std::is_enum<T>::value) {
    using underlying_type = typename std::underlying_type<T>::type;
    underlying_type underlying_value;
    from_bytes_router<underlying_type>(underlying_value, bytes, byte_index,
                                       error_code);
    output = static_cast<T>(underlying_value);
  }
  // std::string
  else if constexpr (detail::is_string::detect<T>) {
    detail::from_bytes(output, bytes, byte_index, error_code);
  }
  // set, unordered_set
  else if constexpr (detail::is_specialization<T, std::set>::value ||
                     detail::is_specialization<T, std::unordered_set>::value) {
    from_bytes_to_set(output, bytes, byte_index, error_code);
  }
  // variant
  else if constexpr (detail::is_specialization<T, std::variant>::value) {
    // current byte is the index of the variant value
    std::size_t index = detail::decode_varint<std::size_t>(bytes, byte_index);

    // read bytes as value_type = variant@index
    detail::set_variant_value<T>(output, index, bytes, byte_index, error_code);
  }
  // vector
  else if constexpr (detail::is_vector<T>::value) {
    from_bytes_to_vector(output, bytes, byte_index, error_code);
  }
  else {
    detail::read_bytes(output, bytes, byte_index, error_code);
  }
}

// Specialization for set

template <typename T>
void from_bytes_to_set(T &set, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index,
                       std::error_code &error_code) {
  // current byte is the size of the set
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  if (size > bytes.size() - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return;
  }

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    typename T::value_type value{};
    from_bytes_router(value, bytes, current_index, error_code);
    set.insert(value);
  }
}

// Specialization for vector

template <typename T>
void from_bytes_to_vector(std::vector<T> &value,
                          const std::vector<uint8_t> &bytes,
                          std::size_t &current_index,
                          std::error_code &error_code) {

  // current byte is the size of the vector
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  if (size > bytes.size() - current_index) {
    // size is greater than the number of bytes remaining
    error_code = std::make_error_code(std::errc::value_too_large);

    // stop here
    return;
  }

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    T v{};
    from_bytes_router(v, bytes, current_index, error_code);
    if (error_code) {
      // something went wrong
      return;
    }
    value.push_back(v);
  }
}

} // namespace detail

/// N -> number of fields in struct
/// I -> field to start from
template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
void deserialize(T &s, const std::vector<uint8_t> &bytes,
                 std::size_t &byte_index, std::error_code &error_code) {
  if constexpr (I < N) {
    decltype(auto) field = detail::get<I, T, N>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // load current field
    detail::from_bytes_router<decayed_field_type>(field, bytes, byte_index,
                                                  error_code);

    if (error_code) {
      // stop here
      return;
    } else {
      // go to next field
      deserialize<T, N, I + 1>(s, bytes, byte_index, error_code);
    }
  }
}

template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
T deserialize(const std::vector<uint8_t> &bytes, std::error_code &error_code) {
  T object{};
  std::size_t byte_index = 0;
  deserialize<T, N, I>(object, bytes, byte_index, error_code);
  return object;
}

// Overloads to check crc in bytes
template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
void deserialize(T &s, const std::vector<uint8_t> &bytes,
                 std::size_t &byte_index, std::error_code &error_code,
                 bool check_crc) {
  if (check_crc) {
    // bytes must be at least 4 bytes long
    if (bytes.size() < 4) {
      error_code = std::make_error_code(std::errc::invalid_argument);
      return;
    } else {
      // check crc bytes
      uint32_t trailing_crc;
      std::size_t index = bytes.size() - 4;
      detail::read_bytes<uint32_t>(trailing_crc, bytes,
                                    index,
                                    error_code); // last 4 bytes

      auto computed_crc = crc32_fast(bytes.data(), bytes.size() - 4);

      if (trailing_crc == computed_crc) {
        // message is good!
        // copy over all bytes except last 4
        // TODO: Should this function accept a non-const vector?
        // If it did, it could just remove the last 4 bytes
        const std::vector<uint8_t> bytes_without_crc(bytes.begin(),
                                                     bytes.begin() + index);
        deserialize<T, N, I>(s, bytes_without_crc, byte_index, error_code);
      } else {
        // message is bad
        error_code = std::make_error_code(std::errc::bad_message);
        return;
      }
    }
  } else {
    // bytes does not have any CRC
    // just deserialize everything into type T
    deserialize<T, N, I>(s, bytes, byte_index, error_code);
  }
}

template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
T deserialize(const std::vector<uint8_t> &bytes, std::error_code &error_code,
              bool check_crc) {
  T object{};
  std::size_t byte_index = 0;
  deserialize<T, N, I>(object, bytes, byte_index, error_code, check_crc);
  return object;
}

} // namespace alpaca