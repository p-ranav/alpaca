#pragma once
#include <serialize/detail/aggregate_arity.h>
#include <serialize/detail/append.h>
#include <serialize/detail/from_bytes.h>
#include <serialize/detail/get_repr_type.h>
#include <serialize/detail/is_pair.h>
#include <serialize/detail/is_string.h>
#include <serialize/detail/is_tuple.h>
#include <serialize/detail/is_vector.h>
#include <serialize/detail/print_bytes.h>
#include <serialize/detail/struct_nth_field.h>
#include <serialize/detail/to_bytes.h>
#include <serialize/detail/type.h>
#include <serialize/detail/variable_length_encoding.h>

template <typename T, std::size_t index = 0>
void serialize(T &s, std::vector<uint8_t> &bytes) {
  constexpr static auto max_index =
      detail::aggregate_arity<std::remove_cv_t<T>>::size();
  if constexpr (index < max_index) {
    decltype(auto) field = detail::get<index>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;

    // check if vector
    if constexpr (detail::is_vector<decayed_field_type>::value) {
      detail::to_bytes_from_list_type<false, decayed_field_type>(field, bytes);
    }
    // check if tuple
    else if constexpr (detail::is_tuple<decayed_field_type>::value) {
      detail::to_bytes_from_tuple_type<false, decayed_field_type>(field, bytes);

    }
    /// TODO: change to <false, decayed_field_type>
    
    
    // check if pair
    else if constexpr (detail::is_pair<decayed_field_type>::value) {
      /// TODO: change to <false, decayed_field_type>
      detail::to_bytes_from_pair_type<true, decayed_field_type>(field, bytes);
    }
    // check if string
    else if constexpr (detail::is_string::detect<decayed_field_type>) {
      detail::to_bytes<false, true>(field, bytes);
    }
    // check if nested struct
    else if constexpr (std::is_class<decayed_field_type>::value) {
      // recurse
      serialize<decayed_field_type>(field, bytes);
    } else {
      if constexpr (std::is_same_v<decayed_field_type, bool>) {
        detail::to_bytes<false, true>(field, bytes);
      } else if constexpr (std::is_same_v<decayed_field_type, int8_t> ||
                           std::is_same_v<decayed_field_type, int16_t> ||
                           std::is_same_v<decayed_field_type, int32_t> ||
                           std::is_same_v<decayed_field_type, int64_t>) {
        // attempt to compress and save type_info
        detail::to_bytes<true, true>(field, bytes);
      } else {
        // no type_info needed
        detail::to_bytes<false, true>(field, bytes);
      }
    }

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