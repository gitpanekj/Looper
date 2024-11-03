#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lts_constructor/lts_constructor.hpp"

namespace py = pybind11;

PYBIND11_MODULE(lts, m)
{
    m.doc() = "lts_constructor";
    py::module_::import("bindings.graphs");

    py::class_<LLVM_Module>(m, "LLVM_Module")
        .def(py::init<>())
        .def("load_module", &LLVM_Module::load_module)
        .def("get_lts", &LLVM_Module::get_lts)
        .def("get_functions", &LLVM_Module::get_functions);
}