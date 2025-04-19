#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <utility>

#include "labeled_transition_system/labeled_transition_system.hpp"
#include "labeled_transition_system/lts_labels.hpp"
#include "expression/predicate.hpp"
#include "expression/expression.hpp"
#include "directed_labeled_graph/pybind_template_register.hpp"

namespace py = pybind11;
using namespace directed_labeled_graph;
using namespace labeled_transition_system;



PYBIND11_MODULE(labeled_transition_system, m)
{
    m.doc() = "Labeled Transition System module";

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


    // DirectedLabeledGraph
    register_dlg<std::string, LTSTransitionLabel>(m, "LTSBase");

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
}
