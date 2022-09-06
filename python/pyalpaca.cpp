#include <string>
#include <iostream>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace alpaca {

namespace python {

void serialize(const std::string& format, py::list &bytes) {
    std::cout << format << "\n";
}

}

}

PYBIND11_MODULE(pyalpaca, m)
{
  m.def("serialize", &alpaca::python::serialize);
}