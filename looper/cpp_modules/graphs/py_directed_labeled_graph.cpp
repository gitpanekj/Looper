#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "graphs/directed_labeled_graph.hpp"
#include "graphs/labeled_transition_system.hpp"
namespace py = pybind11;
using namespace graphs;


// trampoline class - https://pybind11.readthedocs.io/en/stable/advanced/classes.html
class PyDLG : public DirectedLabeledGraph<std::string, std::string> {
public:
    /* Inherit the constructors */
    using DirectedLabeledGraph::DirectedLabeledGraph;

    /* Trampoline (need one for each virtual function) */
    std::string convert_to_dot() override {
        PYBIND11_OVERRIDE_PURE(
            std::string,                /* Return type */
            DirectedLabeledGraph,       /* Parent class */
            convert_to_dot,             /* Name of function in C++ (must match Python name) */
            /* ... Arguments */
        );
    }
};


PYBIND11_MODULE(graphs, m)
{
    m.doc() = "graph module";

    // Directed Labeled Graph
    py::class_<DirectedLabeledGraph<std::string, std::string>>(m, "DirectedLabeledGraph")
        .def(py::init<>())
        .def("add_node", static_cast<int (DirectedLabeledGraph<std::string, std::string>::*)()>                     (&DirectedLabeledGraph<std::string, std::string>::add_node))
        .def("add_node", static_cast<int (DirectedLabeledGraph<std::string, std::string>::*)(std::string)>          (&DirectedLabeledGraph<std::string, std::string>::add_node))
        .def("delete_node",                                                                                         (&DirectedLabeledGraph<std::string, std::string>::delete_node))
        .def("set_node_data",                                                                                       (&DirectedLabeledGraph<std::string, std::string>::set_node_data))
        .def("reset_node_data",                                                                                     (&DirectedLabeledGraph<std::string, std::string>::reset_node_data))
        .def("update_node",                                                                                         (&DirectedLabeledGraph<std::string, std::string>::update_node))
        .def("get_node_data",                                                                                       (&DirectedLabeledGraph<std::string, std::string>::get_node_data))
        .def("get_edges_between",                                                                                   (&DirectedLabeledGraph<std::string, std::string>::get_edges_between))
        .def("add_edge", static_cast<int (DirectedLabeledGraph<std::string, std::string>::*)(int, int)>             (&DirectedLabeledGraph<std::string, std::string>::add_edge))
        .def("add_edge", static_cast<int (DirectedLabeledGraph<std::string, std::string>::*)(int, int, std::string)>(&DirectedLabeledGraph<std::string, std::string>::add_edge))
        .def("delete_edge",                                                                                         (&DirectedLabeledGraph<std::string, std::string>::delete_edge))
        .def("set_edge_data",                                                                                       (&DirectedLabeledGraph<std::string, std::string>::set_edge_data))
        .def("reset_edge_data",                                                                                     (&DirectedLabeledGraph<std::string, std::string>::reset_edge_data))
        .def("get_edge_data",                                                                                       (&DirectedLabeledGraph<std::string, std::string>::get_edge_data))
        .def("get_edge_nodes",                                                                                      (&DirectedLabeledGraph<std::string, std::string>::get_edge_nodes))
        .def("update_edge",                                                                                         (&DirectedLabeledGraph<std::string, std::string>::update_edge))
        .def("convert_to_dot",                                                                                      (&DirectedLabeledGraph<std::string, std::string>::convert_to_dot));

    // Labeled Transition System
    py::class_<LabeledTransitionSystem<std::string, std::string>, DirectedLabeledGraph<std::string, std::string>>(m, "LabeledTransitionSystem")
        .def(py::init<>())
        .def("add_start_location", static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)(std::string)>          (&LabeledTransitionSystem<std::string, std::string>::add_start_location))
        .def("add_start_location", static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)()>                     (&LabeledTransitionSystem<std::string, std::string>::add_start_location))
        .def("add_end_location",   static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)(std::string)>          (&LabeledTransitionSystem<std::string, std::string>::add_end_location))
        .def("add_end_location",   static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)()>                     (&LabeledTransitionSystem<std::string, std::string>::add_end_location))
        .def("add_location",       static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)(std::string)>          (&LabeledTransitionSystem<std::string, std::string>::add_location))
        .def("add_location",       static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)()>                     (&LabeledTransitionSystem<std::string, std::string>::add_location))
        .def("add_transition",     static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)(int, int, std::string)>(&LabeledTransitionSystem<std::string, std::string>::add_transition))
        .def("add_transition",     static_cast<int (LabeledTransitionSystem<std::string, std::string>::*)(int, int)>             (&LabeledTransitionSystem<std::string, std::string>::add_transition))
        .def("convert_to_dot",                                                                                                   (&LabeledTransitionSystem<std::string, std::string>::convert_to_dot));
}
