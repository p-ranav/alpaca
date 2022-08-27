#pragma once
#include <system_error>
#include <utility>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T, typename U, typename V>
void to_bytes(T &bytes, const std::pair<U, V> &input) {
    to_bytes_router(input.first, bytes);
    to_bytes_router(input.second, bytes);
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T, typename U>
bool from_bytes(std::pair<T, U> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {
    from_bytes_router(output.first, bytes, byte_index, error_code);
    from_bytes_router(output.second, bytes, byte_index, error_code);

    return true;
}

}

}