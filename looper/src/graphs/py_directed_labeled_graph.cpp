#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <utility>
#include "graphs/directed_labeled_graph.hpp"
#include "graphs/labeled_transition_system.hpp"
#include "graphs/lts_labels.hpp"
#include "graphs/difference_constraint_program.hpp"
#include "graphs/dcp_labels.hpp"
#include "graphs/variable_flow_graph.hpp"
#include "graphs/lts_to_dcp_mapper.hpp"
#include "expression/predicate.hpp"
#include "expression/expression.hpp"

namespace py = pybind11;
using namespace graphs;

// // trampoline class - https://pybind11.readthedocs.io/en/stable/advanced/classes.html
// class PyDLG : public DirectedLabeledGraph<std::string, LTSTransitionLabel> {
// public:
//     /* Inherit the constructors */
//     using DirectedLabeledGraph::DirectedLabeledGraph;

//     /* Trampoline (need one for each virtual function) */
//     std::string convert_to_dot() const override {
//         PYBIND11_OVERRIDE_PURE(
//             std::string,                /* Return type */
//             DirectedLabeledGraph,       /* Parent class */
//             convert_to_dot,             /* Name of function in C++ (must match Python name) */
//             /* ... Arguments */
//         );
//     }
// };


template<typename VertexDataType, typename EdgeDataType>
void register_dlg(py::module &m, const std::string& name){
     // Directed Labeled Graph
     py::class_<DirectedLabeledGraph<VertexDataType, EdgeDataType>>(m, name.c_str())
     .def(py::init<>())
     .def("add_node", static_cast<int (DirectedLabeledGraph<VertexDataType, EdgeDataType>::*)()>(&DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_vertex))
     .def("add_node", static_cast<int (DirectedLabeledGraph<VertexDataType, EdgeDataType>::*)(VertexDataType)>(&DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_vertex))
     .def("delete_node", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::delete_vertex))
     .def("set_node_data", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::set_vertex_data))
     .def("clear_node_data", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::clear_vertex_data))
     .def("get_node_data", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_vertex_data))
     .def("get_edges_between", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_edges_between))
     .def("add_edge", static_cast<int (DirectedLabeledGraph<VertexDataType, EdgeDataType>::*)(int, int)>(&DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_edge))
     .def("add_edge", static_cast<int (DirectedLabeledGraph<VertexDataType, EdgeDataType>::*)(int, int, EdgeDataType)>(&DirectedLabeledGraph<VertexDataType, EdgeDataType>::add_edge))
     .def("delete_edge", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::delete_edge))
     .def("set_edge_data", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::set_edge_data))
     .def("clear_edge_data", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::clear_edge_data))
     .def("get_edge_data", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_edge_data))
     .def("get_edge_nodes", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_edge_nodes))
     .def("convert_to_dot", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::convert_to_dot))
     .def("get_edges", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_edges))
     .def("get_nodes", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_nodes))
     .def("mark_as_erased", &DirectedLabeledGraph<VertexDataType, EdgeDataType>::mark_as_erased)
     .def("unmark_as_erased", &DirectedLabeledGraph<VertexDataType, EdgeDataType>::unmark_as_erased)
     .def("sccs", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::strongly_connected_components))
     .def("get_successors", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_successors))
     .def("get_predecessors", (&DirectedLabeledGraph<VertexDataType, EdgeDataType>::get_predecessors))
     .def("get_ingoing_edges", [](const DirectedLabeledGraph<VertexDataType, EdgeDataType>& graph, int dst) {
        return py::cast(graph.get_ingoing_edges(dst));
    })
    .def("get_outgoing_edges", [](const DirectedLabeledGraph<VertexDataType, EdgeDataType>& graph, int src) {
        return py::cast(graph.get_outgoing_edges(src));
    });    
}

PYBIND11_MODULE(graphs, m)
{
    m.doc() = "graph module";

    py::class_<LTSTransitionAssignment>(m, "LTSTransitionAssignment")
        .def(py::init<std::string, std::shared_ptr<Expression>>())
        .def("__repr__", &LTSTransitionAssignment::to_string)
        .def_readwrite("lhs", &LTSTransitionAssignment::lhs)
        .def_readwrite("rhs", &LTSTransitionAssignment::rhs);

    py::class_<LTSTransitionCondition>(m, "LTSTransitionCondition")
        .def(py::init<std::shared_ptr<Predicate>, bool>())
        .def(py::init<std::vector<std::shared_ptr<Predicate>>, bool>())
        .def(py::init<>())
        .def_readwrite("condition", &LTSTransitionCondition::condition)
        .def_readwrite("true_branch", &LTSTransitionCondition::true_branch )
        .def("add_condition", static_cast<void (LTSTransitionCondition::*)(std::shared_ptr<Predicate>)>(&LTSTransitionCondition::add_condition))
        .def("add_condition", static_cast<void (LTSTransitionCondition::*)(std::vector<std::shared_ptr<Predicate>>)>(&LTSTransitionCondition::add_condition))
        .def("get_norm", &LTSTransitionCondition::get_norm)
        .def("__repr__", &LTSTransitionCondition::to_string); // Bind the toStr method;

    py::class_<LTSTransitionLabel>(m, "LTSTransitionLabel")
        .def(py::init<>())                                                             // Default constructor
        .def(py::init<LTSTransitionCondition, std::vector<LTSTransitionAssignment>>()) // Constructor with condition and statements
        .def(py::init<std::vector<LTSTransitionAssignment>>())                         // Constructor with only statements
        .def_readwrite("condition", &LTSTransitionLabel::condition)
        .def_readwrite("statements", &LTSTransitionLabel::assignments)
        .def("__repr__", &LTSTransitionLabel::to_string); // Bind the toStr method

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

    // DirectedLabeledGraph
    register_dlg<std::string, LTSTransitionLabel>(m, "LTSBase");
    register_dlg<std::string, DCPTransitionLabel>(m, "DCPBase");
    register_dlg<std::pair<int, std::string>, std::string>(m, "VFGBase");

    // Labeled Transition System
    py::class_<LabeledTransitionSystem, DirectedLabeledGraph<std::string, LTSTransitionLabel>>(m, "LabeledTransitionSystem")
        .def(py::init<>())
        .def("add_start_location", static_cast<int (LabeledTransitionSystem::*)(std::string)>(&LabeledTransitionSystem::add_start_location))
        .def("add_start_location", static_cast<int (LabeledTransitionSystem::*)()>(&LabeledTransitionSystem::add_start_location))
        .def("add_end_location", static_cast<int (LabeledTransitionSystem::*)(std::string)>(&LabeledTransitionSystem::add_end_location))
        .def("add_end_location", static_cast<int (LabeledTransitionSystem::*)()>(&LabeledTransitionSystem::add_end_location))
        .def("mark_loop_head", &LabeledTransitionSystem::mark_loop_head)
        .def("unmark_loop_head", &LabeledTransitionSystem::unmark_loop_head)
        .def("mark_back_edge", &LabeledTransitionSystem::mark_back_edge)
        .def("unmark_back_edge", &LabeledTransitionSystem::unmark_back_edge)
        .def("is_loop_head", &LabeledTransitionSystem::is_loop_head)
        .def("is_back_edge", &LabeledTransitionSystem::is_back_edge)
        .def("get_start_location", &LabeledTransitionSystem::get_start_location)
        .def("get_end_location", &LabeledTransitionSystem::get_end_location)
        .def("get_back_edges", &LabeledTransitionSystem::get_back_edges)
        .def("get_loop_heads", &LabeledTransitionSystem::get_loop_heads)
        .def("convert_to_dot", (&LabeledTransitionSystem::convert_to_dot));

    py::class_<DifferenceConstraintProgram, DirectedLabeledGraph<std::string, DCPTransitionLabel>>(m, "DifferenceConstraintProgram")
        .def(py::init<>())
        .def("get_parameters", &DifferenceConstraintProgram::get_parameters)
        .def("get_back_edges", &DifferenceConstraintProgram::get_back_edges)
        .def("get_start_location", &DifferenceConstraintProgram::get_start_location)
        .def("get_end_location", &DifferenceConstraintProgram::get_end_location)
        .def("is_loop_head", &DifferenceConstraintProgram::is_loop_head)
        .def("is_back_edge", &DifferenceConstraintProgram::is_back_edge)
        .def("convert_to_dot", (&DifferenceConstraintProgram::convert_to_dot));

    py::class_<VariableFlowGraph, DirectedLabeledGraph<std::pair<int, std::string>, std::string>>(m, "VariableFlowGraph")
    .def(py::init<>())
    .def("add_variable_vertex", &VariableFlowGraph::add_variable_vertex)
    .def("delete_variable_vertex", &VariableFlowGraph::delete_variable_vertex)
    .def("find_variable_vertex", &VariableFlowGraph::find_variable_vertex)
    .def("convert_to_dot", (&VariableFlowGraph::convert_to_dot));

    m.def("lts_to_dcp", &lts_to_dcp_mapper);
}
