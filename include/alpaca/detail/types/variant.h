#pragma once
#include <alpaca/detail/variable_length_encoding.h>
#include <alpaca/detail/variant_nth_field.h>
#include <system_error>
#include <variant>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T, typename... U>
void to_bytes(T &bytes, const std::variant<U...> &input) {
    std::size_t index = input.index();

    // save index of variant
    to_bytes_router<std::size_t>(index, bytes);

    // save value of variant
    const auto visitor = [&bytes](auto &&arg) { to_bytes_router(arg, bytes); };
    std::visit(visitor, input);
}

template <typename... T>
bool from_bytes(std::variant<T...> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
    // current byte is the index of the variant value
    std::size_t index = detail::decode_varint<std::size_t>(bytes, byte_index);

    // read bytes as value_type = variant@index
    detail::set_variant_value<std::variant<T...>>(output, index, bytes, byte_index, error_code);

    return true;
}

}

}