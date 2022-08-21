#pragma once
#include <serialize/detail/type.h>

namespace detail {

template <typename T, typename U> void append(const T &value, U &bytes) {
  std::copy(static_cast<const char *>(static_cast<const void *>(&value)),
            static_cast<const char *>(static_cast<const void *>(&value)) +
                sizeof value,
            std::back_inserter(bytes));
}

template <typename U> void append(type type, U &bytes) {
  append(static_cast<uint8_t>(type), bytes);
}

} // namespace detail