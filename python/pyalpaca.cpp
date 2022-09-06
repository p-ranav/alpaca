#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <alpaca/alpaca.h>

namespace py = pybind11;

namespace alpaca {

namespace python {

// char c
// int8_t b
// uint8_t B
// bool ?
// int16_t h
// uint16_t H
// int32_t i
// uint32_t I
// int64_t q
// uint64_t Q
// size_t N
// float f
// double d

// string s
// vector v
// array a
// map m
// unordered_map M
// set e
// unordered_set E
// tuple t

decltype(auto) value_type(char CODE) {
    if (CODE == 'c') {
        return char{};
    }
}

py::list serialize(const std::string& format, py::list &args) {
    std::vector<uint8_t> result;
    std::size_t byte_index = 0;

    constexpr auto OPTIONS = alpaca::options::none;

    std::size_t index = 0;

    for(auto it = args.begin(); it != args.end(); ++it) {
        if (format[index] == '?') {
            // field is a bool
            detail::to_bytes_router<OPTIONS>(it->cast<bool>(), result, byte_index);
        }
        else if (format[index] == 'c') {
            // field is a character
            detail::to_bytes_router<OPTIONS>(it->cast<char>(), result, byte_index);
        }
        else if (format[index] == 'b') {
            // field is a int8_t
            detail::to_bytes_router<OPTIONS>(it->cast<int8_t>(), result, byte_index);
        }
        else if (format[index] == 'B') {
            // field is a uint8_t
            detail::to_bytes_router<OPTIONS>(it->cast<uint8_t>(), result, byte_index);
        }
        else if (format[index] == 'h') {
            // field is a int16_t
            detail::to_bytes_router<OPTIONS>(it->cast<int16_t>(), result, byte_index);
        }
        else if (format[index] == 'H') {
            // field is a uint16_t
            detail::to_bytes_router<OPTIONS>(it->cast<uint16_t>(), result, byte_index);
        }
        else if (format[index] == 'i') {
            // field is a int32_t
            detail::to_bytes_router<OPTIONS>(it->cast<int32_t>(), result, byte_index);
        }
        else if (format[index] == 'I') {
            // field is a uint32_t
            detail::to_bytes_router<OPTIONS>(it->cast<uint32_t>(), result, byte_index);
        }
        else if (format[index] == 'q') {
            // field is a int64_t
            detail::to_bytes_router<OPTIONS>(it->cast<int64_t>(), result, byte_index);
        }
        else if (format[index] == 'Q') {
            // field is a uint64_t
            detail::to_bytes_router<OPTIONS>(it->cast<uint64_t>(), result, byte_index);
        }
        else if (format[index] == 'f') {
            // field is a float
            detail::to_bytes_router<OPTIONS>(it->cast<float>(), result, byte_index);
        }
        else if (format[index] == 'd') {
            // field is a double
            detail::to_bytes_router<OPTIONS>(it->cast<double>(), result, byte_index);
        }
        else if (format[index] == 'N') {
            // field is a std::size_t
            detail::to_bytes_router<OPTIONS>(it->cast<std::size_t>(), result, byte_index);
        }
        else if (format[index] == 's') {
            // field is a std::string
            detail::to_bytes_router<OPTIONS>(it->cast<std::string>(), result, byte_index);
        }
        else if (format[index] == 'v') {
            // field is a std::vector
            // read subtype
            index += 1;
            auto value_type = format[index];
            // TODO: figure out how to get type of value_type and pass to serializer
            // detail::to_bytes_router<OPTIONS>(it->cast<std::vector<>>(), result, byte_index);
        }

        index += 1;
    }

    // cast as py::list
    py::list pyresult = py::cast(result);
    return pyresult;
}

}

}

PYBIND11_MODULE(pyalpaca, m)
{
  m.def("serialize", &alpaca::python::serialize);
}