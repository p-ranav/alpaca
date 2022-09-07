#include "python_serialize_helper.h"

PYBIND11_MODULE(pyalpaca, m) {
  m.def("serialize", &alpaca::python::do_serialize);
}