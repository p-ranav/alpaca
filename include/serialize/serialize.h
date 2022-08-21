#pragma once
#include <boost/pfr.hpp>
#include <serialize/detail/aggregate_arity.h>
#include <serialize/detail/append.h>
#include <serialize/detail/append_value_type.h>
#include <serialize/detail/get_repr_type.h>
#include <serialize/detail/is_pair.h>
#include <serialize/detail/is_string.h>
#include <serialize/detail/is_tuple.h>
#include <serialize/detail/is_vector.h>
#include <serialize/detail/struct_nth_field.h>
#include <serialize/detail/to_bytes.h>
#include <serialize/detail/type.h>

template <typename T, std::size_t index = 0>
void serialize(T &s, std::vector<uint8_t> &bytes) {
  constexpr static auto max_index =
      detail::aggregate_arity<std::remove_cv_t<T>>::size();
  if constexpr (index < max_index) {
    decltype(auto) field = detail::get<index>(s);
    using decayed_field_type = typename std::decay<decltype(field)>::type;
    // using decayed_field_type =
    //     typename std::decay<decltype(boost::pfr::get<index>(s))>::type;
    if constexpr (detail::is_vector<decayed_field_type>::value) {
      detail::to_bytes_from_list_type<true, decayed_field_type>(
          boost::pfr::get<index>(s), bytes);
    } else if constexpr (detail::is_tuple<decayed_field_type>::value) {
      detail::to_bytes_from_tuple_type<true, decayed_field_type>(
          boost::pfr::get<index>(s), bytes);

    } else if constexpr (detail::is_pair<decayed_field_type>::value) {
      detail::to_bytes_from_pair_type<true, decayed_field_type>(
          boost::pfr::get<index>(s), bytes);
    } else if constexpr (detail::is_string::detect<decayed_field_type>) {
      detail::to_bytes<true, true>(boost::pfr::get<index>(s), bytes);
    } else if constexpr (std::is_class<decayed_field_type>::value) {
      // save nested struct type
      detail::append(detail::type::struct_, bytes);

      // recurse
      serialize<decayed_field_type>(field, bytes);
    } else {
      detail::to_bytes<true, true>(boost::pfr::get<index>(s), bytes);
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
