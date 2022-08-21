#pragma once
#include <utility>

namespace detail {

struct filler {
  template <typename type> operator type();
};

template <typename aggregate, typename index_sequence = std::index_sequence<>,
          typename = void>
struct aggregate_arity : index_sequence {};

template <typename aggregate, std::size_t... indices>
struct aggregate_arity<
    aggregate, std::index_sequence<indices...>,
    std::void_t<decltype(aggregate{(void(indices), std::declval<filler>())...,
                                   std::declval<filler>()})>>
    : aggregate_arity<aggregate,
                      std::index_sequence<indices..., sizeof...(indices)>> {};

} // namespace detail