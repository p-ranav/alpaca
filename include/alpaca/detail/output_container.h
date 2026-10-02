#pragma once
#include <array>
#include <fstream>
#include <system_error>
#include <vector>

namespace alpaca {

namespace detail {

static inline void append(const uint8_t &value, std::vector<uint8_t> &container,
                          std::size_t &index) {
  container.push_back(value);
  index += 1;
}

template <std::size_t N>
void append(const uint8_t &value, std::array<uint8_t, N> &container,
            std::size_t &index) {
  container[index++] = value;
}

static inline void append(const uint8_t &value, uint8_t container[],
                          std::size_t &index) {
  container[index++] = value;
}

static inline void append(const uint8_t &value, std::ofstream &container,
                          std::size_t &index) {
  container << value;
  index += 1;
}

// returns a raw pointer to the underlying bytes, regardless of whether
// Container is a type with `.data()` (vector/array), a C-style array, or a
// raw pointer (e.g. deserializing from a buffer that isn't owned by alpaca)
template <typename Container>
auto container_data(Container &bytes) -> decltype(bytes.data()) {
  return bytes.data();
}

template <typename T>
T *container_data(T *bytes) {
  return bytes;
}

template <typename T, std::size_t N>
T *container_data(T (&bytes)[N]) {
  return bytes;
}

} // namespace detail

} // namespace alpaca
