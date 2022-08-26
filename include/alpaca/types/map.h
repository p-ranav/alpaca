#pragma once
#include <map>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace alpaca {

namespace detail {

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

template <typename T>
void from_bytes_to_map(T &map, const std::vector<uint8_t> &bytes,
                       std::size_t &current_index,
                       std::error_code &error_code) {
  // current byte is the size of the map
  std::size_t size = detail::decode_varint<std::size_t>(bytes, current_index);

  // read `size` bytes and save to value
  for (std::size_t i = 0; i < size; ++i) {
    typename T::key_type key{};
    from_bytes_router(key, bytes, current_index, error_code);

    typename T::mapped_type value{};
    from_bytes_router(value, bytes, current_index, error_code);

    map.insert(std::make_pair(key, value));
  }
}



}

}