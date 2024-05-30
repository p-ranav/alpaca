#pragma once
#ifdef ALPACA_INCLUDE_SUPPORT_GLM_VECTOR
#include <alpaca/detail/types/array.h>

#include <glm/ext/vector_float3.hpp>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace alpaca {

namespace detail {


template <typename T> struct is_glm_vec : std::false_type {};

template <int L, typename T, glm::qualifier Q>
struct is_glm_vec<glm::vec<L, T, Q>> : std::true_type {};

template <typename T>
typename std::enable_if<is_glm_vec<T>::value, void>::type
type_info(
    std::vector<uint8_t> &typeids,
    std::unordered_map<std::string_view, std::size_t> &struct_visitor_map) {
	type_info<std::array<typename T::T, T::L>>(typeids, struct_visitor_map);
}

template <options O, typename Container, int L, typename T, glm::qualifier Q>
void to_bytes(Container &bytes, std::size_t &byte_index, const glm::vec<L, T, Q> &input) {
    std::array<T, L> data;
	for (int i = 0; i < L; i++)
		data[i] = input[i];

    to_bytes<O, Container, T, L>(bytes, byte_index, data);
}

template <options O, int L, typename T, glm::qualifier Q, typename Container>
bool from_bytes(glm::vec<L, T, Q> &output, Container &bytes, std::size_t &byte_index, std::size_t &end_index,
                std::error_code &error_code) {
    std::array<T, L> output_array;
    from_bytes<O, T, Container, L>(output_array, bytes, byte_index, end_index, error_code);

	for (int i = 0; i < L; i++)
		output[i] = output_array[i];

    return true;
}

}  // namespace detail

}  // namespace alpaca
#endif
