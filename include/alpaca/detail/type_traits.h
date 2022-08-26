#pragma once
#include <memory>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace alpaca {

namespace detail {

// check if T is instantiation of U
template <typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

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

// vector
template <typename T> struct is_vector : public std::false_type {};

template <typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

} // namespace detail

} // namespace alpaca