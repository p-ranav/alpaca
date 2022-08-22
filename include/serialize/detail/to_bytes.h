#pragma once
#include <cstring>
#include <serialize/detail/is_string.h>
#include <serialize/detail/unsigned_int_encoding.h>

namespace detail {

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint8_t input, std::vector<uint8_t> &bytes) {
  encode_varint<uint8_t>(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint16_t input, std::vector<uint8_t> &bytes) {
  encode_varint<uint16_t>(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint32_t input, std::vector<uint8_t> &bytes) {
  encode_varint<uint32_t>(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(uint64_t input, std::vector<uint8_t> &bytes) {
  encode_varint<uint64_t>(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(bool input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(type::boolean, bytes);
  }
  // value
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(int8_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(type::int8, bytes);
  }
  // value
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(char input, std::vector<uint8_t> &bytes) {
  to_bytes<save_type_info, attempt_compression>(static_cast<int8_t>(input),
                                                bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(int16_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<int8_t>::max() &&
        input >= std::numeric_limits<int8_t>::min()) {
      // value can fit in an int8_t
      append(static_cast<int8_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(int32_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<int16_t>::max() &&
        input >= std::numeric_limits<int16_t>::min()) {
      // value can fit in an int16_t
      to_bytes<false, true>(static_cast<int16_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(int64_t input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(get_repr_type<attempt_compression>(input), bytes);
  }

  if constexpr (attempt_compression) {
    if (input <= std::numeric_limits<int32_t>::max() &&
        input >= std::numeric_limits<int32_t>::min()) {
      // value can fit in an int32_t
      to_bytes<false, true>(static_cast<int32_t>(input), bytes);
    } else {
      // value
      append(input, bytes);
    }
  } else {
    // value
    append(input, bytes);
  }
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(const float input, std::vector<uint8_t> &bytes) {
  if constexpr (save_type_info) {
    append(type::float32, bytes);
  }
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(const double input, std::vector<uint8_t> &bytes) {
  if constexpr (save_type_info) {
    append(type::float64, bytes);
  }
  append(input, bytes);
}

template <bool save_type_info, bool attempt_compression>
void to_bytes(const std::string &input, std::vector<uint8_t> &bytes) {
  if constexpr (save_type_info) {
    append(type::string, bytes);
  }

  // save string length
  to_bytes<true, true>(input.size(), bytes);

  for (auto &c : input) {
    append(c, bytes);
  }
}

template <typename T, std::size_t index>
void save_tuple_value(const T &tuple, std::vector<uint8_t> &bytes) {
  constexpr auto max_index = std::tuple_size<T>::value;
  if constexpr (index < max_index) {
    to_bytes<false, false>(std::get<index>(tuple), bytes);
    save_tuple_value<T, index + 1>(tuple, bytes);
  }
}

template <bool save_type_info, typename T>
void to_bytes_from_tuple_type(const T &input, std::vector<uint8_t> &bytes) {
  // type of tuple + each element
  if constexpr (save_type_info) {
    append(type::tuple, bytes);
  }

  // value of each element
  save_tuple_value<T, 0>(input, bytes);
}

template <typename T>
void save_pair_value(const T &pair, std::vector<uint8_t> &bytes) {
  to_bytes<false, false>(pair.first, bytes);
  to_bytes<false, false>(pair.second, bytes);
}

template <bool save_type_info, typename T>
void to_bytes_from_pair_type(const T &input, std::vector<uint8_t> &bytes) {
  // type of tuple + each element
  if constexpr (save_type_info) {
    append(type::pair, bytes);
  }

  // value of each element
  save_pair_value<T>(input, bytes);
}

template <bool save_type_info, typename T>
void to_bytes_from_list_type(const T &input, std::vector<uint8_t> &bytes) {
  // type of the value
  if constexpr (save_type_info) {
    append(type::vector, bytes);
  }

  // save vector size
  to_bytes<true, true>(input.size(), bytes);

  // value of each element in list
  for (const auto &v : input) {
    // check if the value_type is a nested list type
    using decayed_value_type = typename std::decay<decltype(v)>::type;
    if constexpr (is_vector<decayed_value_type>::value) {
      to_bytes_from_list_type<false, decayed_value_type>(v, bytes);
    } else if constexpr (is_tuple<decayed_value_type>::value) {
      to_bytes_from_tuple_type<false, decayed_value_type>(v, bytes);
    } else if constexpr (is_string::detect<decayed_value_type>) {
      to_bytes<false, false>(v, bytes);
    } else if constexpr (std::is_same_v<decayed_value_type, uint8_t> ||
                         std::is_same_v<decayed_value_type, uint16_t> ||
                         std::is_same_v<decayed_value_type, uint32_t> ||
                         std::is_same_v<decayed_value_type, uint64_t> ||
                         std::is_same_v<decayed_value_type, std::size_t>) {
      // unsigned integer type
      // use MSB variable-length encoding
      to_bytes<true, true>(v, bytes);
    } else {
      // dump all the values
      // note: no attempted compression for integer types
      append(v, bytes);
    }
  }
}

} // namespace detail