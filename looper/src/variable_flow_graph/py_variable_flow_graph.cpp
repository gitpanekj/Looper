#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <utility>
#include "variable_flow_graph/variable_flow_graph.hpp"
#include "directed_labeled_graph/pybind_template_register.hpp"
#include "expression/expression.hpp"

namespace py = pybind11;
using namespace variable_flow_graph;
using namespace directed_labeled_graph;

PYBIND11_MODULE(variable_flow_graph, m)
{
    m.doc() = "Variable Flow Graph module";

    // DirectedLabeledGraph
    register_dlg<std::pair<int, std::string>, std::string>(m, "VFGBase");

    // Variable Flow Graph
    py::class_<VariableFlowGraph, DirectedLabeledGraph<std::pair<int, std::string>, std::string>>(m, "VariableFlowGraph")
    .def(py::init<>())
    .def("add_variable_vertex", &VariableFlowGraph::add_variable_vertex)
    .def("delete_variable_vertex", &VariableFlowGraph::delete_variable_vertex)
    .def("find_variable_vertex", &VariableFlowGraph::find_variable_vertex)
    .def("convert_to_dot", (&VariableFlowGraph::convert_to_dot));
}
