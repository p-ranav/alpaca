#pragma once
#include <optional>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T, typename U>
void to_bytes(T &bytes, const std::optional<U> &input) {
    const auto has_value = input.has_value();

    // save if optional has value
    to_bytes_router<bool>(has_value, bytes);

    // save value
    if (has_value) {
        to_bytes_router<U>(input.value(), bytes);
    }
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T>
bool from_bytes(std::optional<T> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {

    // current byte is the `has_value` byte
    bool has_value = static_cast<bool>(bytes[byte_index++]);

    if (has_value) {
      // read value of optional
      T value;
      from_bytes_router(value, bytes, byte_index, error_code);
      output = value;
    }

    return true;
}

}

}