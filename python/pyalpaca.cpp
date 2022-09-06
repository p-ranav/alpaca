#include <string>
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <alpaca/alpaca.h>

namespace py = pybind11;

namespace alpaca {

namespace python {

// char c
// uint8_t b
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

py::list serialize(const std::string& format, py::list &args) {
    std::vector<uint8_t> result;
    std::size_t byte_index = 0;

    constexpr auto OPTIONS = alpaca::options::fixed_length_encoding;

    std::size_t index = 0;

    for(auto it = args.begin(); it != args.end(); ++it) {
        if (format[index] == 'c') {
            // field is a character
            detail::to_bytes_router<OPTIONS>(it->cast<char>(), result, byte_index);
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