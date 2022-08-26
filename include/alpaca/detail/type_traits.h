#pragma once
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <tuple>
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

// map

namespace map {
// Needed for some older versions of GCC
template <typename...> struct voider { using type = void; };

// std::void_t will be part of C++17, but until then define it ourselves:
template <typename... T> using void_t = typename voider<T...>::type;

template <typename T, typename U = void>
struct is_mappish_impl : std::false_type {};

template <typename T>
struct is_mappish_impl<T, void_t<typename T::key_type, typename T::mapped_type,
                                 decltype(std::declval<T &>()[std::declval<
                                     const typename T::key_type &>()])>>
    : std::true_type {};
} // namespace map

template <typename T> struct is_mappish : map::is_mappish_impl<T>::type {};

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

} // namespace alpaca