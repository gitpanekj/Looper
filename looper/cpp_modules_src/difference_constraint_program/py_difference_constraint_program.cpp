#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <utility>
#include <memory>
#include "directed_labeled_graph/pybind_template_register.hpp"
#include "difference_constraint_program/difference_constraint_program.hpp"
#include "difference_constraint_program/dcp_labels.hpp"
#include "difference_constraint_program/lts_to_dcp_mapper.hpp"
#include "expression/expression.hpp"

namespace py = pybind11;
using namespace difference_constraint_program;
using namespace directed_labeled_graph;

PYBIND11_MODULE(difference_constraint_program, m)
{
    m.doc() = "Difference constraint program module";


    py::class_<DifferenceConstraint>(m, "DifferenceConstraint")
        .def(py::init<std::shared_ptr<Expression>, std::shared_ptr<Expression>, std::shared_ptr<Expression>>())
        .def("__repr__", &DifferenceConstraint::to_string)
        .def_readwrite("x", &DifferenceConstraint::x)
        .def_readwrite("y", &DifferenceConstraint::y)
        .def_readwrite("c", &DifferenceConstraint::c);

    py::class_<DCPTransitionLabel>(m, "DCPTransitionLabel")
        .def(py::init<>())
        .def("add_dc", &DCPTransitionLabel::add_dc)
        .def("add_guard", &DCPTransitionLabel::add_guard)
        .def("is_in_guards", &DCPTransitionLabel::is_in_guards)
        .def_readwrite("true_branch", &DCPTransitionLabel::true_branch)
        .def_readwrite("constraints", &DCPTransitionLabel::constraints)
        .def_readwrite("guards", &DCPTransitionLabel::guards);

    
    register_dlg<std::string, DCPTransitionLabel>(m, "DCPBase");
    py::class_<DifferenceConstraintProgram, DirectedLabeledGraph<std::string, DCPTransitionLabel>>(m, "DifferenceConstraintProgram")
        .def(py::init<>())
        .def("get_parameters", &DifferenceConstraintProgram::get_parameters)
        .def("get_back_edges", &DifferenceConstraintProgram::get_back_edges)
        .def("get_start_location", &DifferenceConstraintProgram::get_start_location)
        .def("get_end_location", &DifferenceConstraintProgram::get_end_location)
        .def("is_loop_head", &DifferenceConstraintProgram::is_loop_head)
        .def("is_back_edge", &DifferenceConstraintProgram::is_back_edge)
        .def("convert_to_dot", (&DifferenceConstraintProgram::convert_to_dot));

    m.def("lts_to_dcp", &lts_to_dcp_mapper);
}
