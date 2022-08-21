#pragma once
#include <vector>

namespace detail {

template <typename T> struct is_vector : public std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

} // namespace detail