#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lts_constructor/lts_constructor.hpp"

namespace py = pybind11;

PYBIND11_MODULE(lts, m)
{
    m.doc() = "lts_constructor";
    py::module_::import("bindings.graphs");
    py::module_::import("bindings.expression");

    py::class_<LTSConstructor>(m, "LTSConstructor")
        .def(py::init<>())
        .def("load_module", &LTSConstructor::loadModule)
        .def("get_lts", &LTSConstructor::getLTS)
        .def("get_functions", &LTSConstructor::getFunctions);
}