#pragma once
#include <tuple>
#include <utility>

namespace alpaca {

namespace detail {

template <typename T> struct is_single_element_tuple : std::false_type {};
template <typename T>
struct is_single_element_tuple<std::tuple<T>> : std::true_type {};

struct filler {
  template <typename type,
            typename = std::enable_if_t<
                !is_single_element_tuple<type>::value>>
  operator type();
};

template <typename aggregate, typename index_sequence = std::index_sequence<>,
          typename = void>
struct aggregate_arity : index_sequence {};

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

template <typename aggregate, std::size_t... indices>
struct aggregate_arity<
    aggregate, std::index_sequence<indices...>,
    std::void_t<decltype(aggregate{(void(indices), std::declval<filler>())...,
                                   std::declval<filler>()})>>
    : aggregate_arity<aggregate,
                      std::index_sequence<indices..., sizeof...(indices)>> {};

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template <typename... Ts>
struct aggregate_arity<std::tuple<Ts...>> : std::make_index_sequence<sizeof...(Ts)> {};

} // namespace detail


} // namespace alpaca