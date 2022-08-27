#pragma once
#include <alpaca/detail/aggregate_arity.h>
#include <alpaca/detail/crc32.h>
#include <alpaca/detail/endian.h>
#include <alpaca/detail/from_bytes.h>
#include <alpaca/detail/print_bytes.h>
#include <alpaca/detail/struct_nth_field.h>
#include <alpaca/detail/to_bytes.h>
#include <alpaca/detail/type_traits.h>
#include <alpaca/detail/types/array.h>
#include <alpaca/detail/types/map.h>
#include <alpaca/detail/types/optional.h>
#include <alpaca/detail/types/pair.h>
#include <alpaca/detail/types/set.h>
#include <alpaca/detail/types/string.h>
#include <alpaca/detail/types/tuple.h>
#include <alpaca/detail/types/unique_ptr.h>
#include <alpaca/detail/types/variant.h>
#include <alpaca/detail/types/vector.h>
#include <alpaca/detail/variable_length_encoding.h>

#include <system_error>

namespace alpaca {

// Forward declares
template <typename T, std::size_t N, std::size_t I>
void serialize(const T &s, std::vector<uint8_t> &bytes);

namespace detail {

// Start of serialization functions

// version for nested struct/class types
// incidentally, also works for std::pair
template <typename T, typename U>
typename std::enable_if<std::is_aggregate_v<U>, void>::type
to_bytes(T &bytes, const U &input) {
  serialize<U, detail::aggregate_arity<std::remove_cv_t<U>>::size(), 0>(input,
                                                                        bytes);
}

template <typename T, typename U>
typename std::enable_if<!std::is_aggregate_v<U> && std::is_class_v<U>,
                        void>::type
to_bytes(T &bytes, const U &input);

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes) {
  to_bytes(bytes, input);
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

// Overloads that pack CRC32 of bytes

template <typename T,
          std::size_t N = detail::aggregate_arity<std::remove_cv_t<T>>::size(),
          std::size_t I = 0>
void serialize(const T &s, std::vector<uint8_t> &bytes, bool generate_crc) {
  serialize<T, N, I>(s, bytes);

  if (N > 0 && generate_crc) {
    // calculate crc32 for byte array and
    // pack uint32_t to the end
    uint32_t crc = crc32_fast(bytes.data(), bytes.size());
    detail::to_bytes_crc32(bytes, crc);
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

// version for nested struct/class types
template <typename T>
typename std::enable_if<std::is_aggregate_v<T>, bool>::type
from_bytes(T &value, const std::vector<uint8_t> &bytes, std::size_t &byte_index,
       std::error_code &error_code) {
  deserialize<T, detail::aggregate_arity<std::remove_cv_t<T>>::size(), 0>(
      value, bytes, byte_index, error_code);
  return true;
}

// template <typename T>
// typename std::enable_if<!std::is_aggregate_v<T> && std::is_class_v<T>,
//                         bool>::type
// from_bytes(T &value, const std::vector<uint8_t> &bytes, std::size_t &byte_index,
//            std::error_code &error_code);

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code) {
  // unsigned or signed integer types
  // char, bool
  // float, double
  if constexpr (std::is_arithmetic_v<T>) {
    detail::from_bytes(output, bytes, byte_index, error_code);
  }
  // enum class
  else if constexpr (std::is_enum<T>::value) {
    using underlying_type = typename std::underlying_type<T>::type;
    underlying_type underlying_value{};
    from_bytes_router<underlying_type>(underlying_value, bytes, byte_index,
                                       error_code);
    output = static_cast<T>(underlying_value);
  } else {
    detail::from_bytes(output, bytes, byte_index, error_code);
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
      detail::from_bytes_crc32(trailing_crc, bytes, index,
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