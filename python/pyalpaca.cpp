#include "python_deserialize_helper.h"
#include "python_serialize_helper.h"

PYBIND11_MODULE(pyalpaca, m) {
  m.def("serialize", &alpaca::python::do_serialize);
  m.def("deserialize", &alpaca::python::do_deserialize);
}