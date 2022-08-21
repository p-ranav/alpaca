#pragma once
#include <tuple>

namespace detail {

template <typename> struct is_tuple : std::false_type {};

template <typename... T> struct is_tuple<std::tuple<T...>> : std::true_type {};

} // namespace detail