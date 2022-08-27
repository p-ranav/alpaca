#pragma once
#include <alpaca/detail/to_bytes.h>
#include <memory>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void to_bytes_router(const T &input, std::vector<uint8_t> &bytes);

template <typename T, typename U>
void to_bytes(T &bytes, const std::unique_ptr<U> &input) {
    auto has_value = false;
    if (input) {
      has_value = true;
    }

    // save if ptr has value
    to_bytes_router<bool>(has_value, bytes);

    // save value
    if (has_value) {
      to_bytes_router<U>(*input, bytes);
    }
}

template <typename T>
void from_bytes_router(T &output, const std::vector<uint8_t> &bytes,
                       std::size_t &byte_index, std::error_code &error_code);

template <typename T>
bool from_bytes(std::unique_ptr<T> &output, const std::vector<uint8_t> &bytes,
            std::size_t &byte_index,
            std::error_code &error_code) {

    // current byte is the `has_value` byte
    bool has_value = static_cast<bool>(bytes[byte_index++]);

    if (has_value) {
      // read value of unique_ptr
      T value{};
      from_bytes_router(value, bytes, byte_index, error_code);
      output = std::unique_ptr<T>(new T{std::move(value)});
    } else {
      output = nullptr;
    }

    return true;
}

}

}