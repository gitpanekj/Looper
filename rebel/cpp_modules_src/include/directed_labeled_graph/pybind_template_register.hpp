#ifndef PY_DLG
#define PY_DLG

#include <pybind11/pybind11.h>
#include <string>
#include "directed_labeled_graph.hpp"
#include "expression/predicate.hpp"
#include "expression/expression.hpp"

namespace py = pybind11;


namespace directed_labeled_graph {

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
}

#endif