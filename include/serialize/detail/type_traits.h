#pragma once
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace detail {

// pair
template <typename> struct is_pair : std::false_type {};

template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

// string
template <class T> struct tag_t {};
template <class T> constexpr tag_t<T> tag{};
namespace is_string {
template <class T, class... Ts>
constexpr bool is_stringlike(tag_t<T>, Ts &&...) {
  return false;
}
template <class T, class A>
constexpr bool is_stringlike(tag_t<std::basic_string<T, A>>) {
  return true;
}
template <class T>
constexpr bool detect = is_stringlike(tag<T>); // enable ADL extension
} // namespace is_string

// tuple
template <typename> struct is_tuple : std::false_type {};

template <typename... T> struct is_tuple<std::tuple<T...>> : std::true_type {};

// vector
template <typename T> struct is_vector : public std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

} // namespace detail