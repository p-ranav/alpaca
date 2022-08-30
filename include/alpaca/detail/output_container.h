#pragma once
#include <array>
#include <fstream>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

template <typename T>
void append(const T& value, std::vector<uint8_t>& container, std::size_t& index, std::error_code& error_code) {
    container.push_back(value);
    index += 1;
}

template <typename T, std::size_t N>
void append(const T& value, std::array<uint8_t, N>& container, std::size_t& index, std::error_code& error_code) {
    container[index++] = value;
}

template <typename T>
void append(const T& value, std::ofstream& os, std::size_t& index, std::error_code& error_code) {
    os << value;
}

}

}