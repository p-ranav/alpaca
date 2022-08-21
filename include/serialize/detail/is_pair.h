#pragma once
#include <utility>

namespace detail {

template <typename> struct is_pair : std::false_type {};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

} // namespace detail