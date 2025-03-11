#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "lts_constructor/llvm_ir_processor.hpp"

namespace py = pybind11;

PYBIND11_MODULE(lts, m)
{
    m.doc() = "lts_constructor";
    py::module_::import("graphs");
    py::module_::import("expression");


    py::class_<LLVMIRProcessor>(m, "LLVMIRProcessor")
        .def(py::init<>())
        .def("load_module", &LLVMIRProcessor::loadModule)
        .def("get_lts", &LLVMIRProcessor::getLTS)
        .def("get_functions", &LLVMIRProcessor::getFunctions);
}