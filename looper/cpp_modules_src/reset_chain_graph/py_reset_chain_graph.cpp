#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <utility>
#include "reset_chain_graph/reset_chain_graph.hpp"
#include "directed_labeled_graph/pybind_template_register.hpp"
#include "expression/predicate.hpp"
#include "expression/expression.hpp"

namespace py = pybind11;
using namespace directed_labeled_graph;
using namespace reset_chain_graph;


PYBIND11_MODULE(reset_chain_graph, m)
{
    m.doc() = "Reset Chain Graph module";

    register_dlg<std::shared_ptr<Expression>, std::pair<int, std::shared_ptr<Expression>>>(m, "RCBase");

    py::class_<ResetChainGraph, DirectedLabeledGraph<std::shared_ptr<Expression>, std::pair<int, std::shared_ptr<Expression>>>>(m, "ResetChainGraph")
    .def(py::init<>())
    .def("find_atom_vertex", &ResetChainGraph::find_atom_vertex)
    .def("add_atom", &ResetChainGraph::add_atom)
    .def("convert_to_dot", (&ResetChainGraph::convert_to_dot));
}
