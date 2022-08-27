#pragma once
#include <memory>
#include <set>
#include <unordered_set>
#include <variant>
#include <vector>

namespace alpaca {

namespace detail {

// check if T is instantiation of U
template <typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

// vector
template <typename T> struct is_vector : public std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

} // namespace detail

} // namespace alpaca