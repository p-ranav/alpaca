#pragma once
#include <string>

namespace detail {

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

} // namespace detail