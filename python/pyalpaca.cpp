#include <string>
#include <iostream>
#include <pybind11/pybind11.h>

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
    
}

}

}

PYBIND11_MODULE(pyalpaca, m)
{
  m.def("serialize", &alpaca::python::serialize);
}