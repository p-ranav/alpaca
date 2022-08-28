#pragma once
#include <alpaca/detail/field_type.h>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T, std::size_t N>
typename std::enable_if<std::is_aggregate_v<T>, void>::type
type_info(std::vector<uint8_t>& typeids, 
  std::unordered_map<std::string_view, std::size_t>& struct_visitor_map);

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, bool>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::bool_>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, char>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::char_>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, uint8_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::uint8>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, uint16_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::uint16>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, uint32_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::uint32>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, uint64_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::uint64>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, int8_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::int8>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, int16_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::int16>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, int32_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::int32>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, int64_t>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::int64>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, float>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::float32>());
}

template <typename T, std::size_t N>
typename std::enable_if<std::is_same_v<T, double>, void>::type
type_info(std::vector<uint8_t>& typeids, std::unordered_map<std::string_view, std::size_t>&) {
    typeids.push_back(to_byte<field_type::float64>());
}

}

}