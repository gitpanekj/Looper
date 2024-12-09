#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "expression/expression.hpp"
namespace py = pybind11;



PYBIND11_MODULE(expression, m) {
    // Binding Expression base class
    py::class_<expression::Expression, std::shared_ptr<expression::Expression>>(m, "Expression")
        .def("toStr", &expression::Expression::toStr)
        .def("replace", &expression::Expression::replace);


    // Bind the BinaryOperation class
    py::class_<expression::BinaryOperation>(m, "BinaryOperation")
        .def("getFirstOperand", &expression::BinaryOperation::getFirstOperand)
        .def("getSecondOperand", &expression::BinaryOperation::getSecondOperand)
        .def("setFirstOperand", &expression::BinaryOperation::setFirstOperand)
        .def("setSecondOperand", &expression::BinaryOperation::setSecondOperand);


    // Binding the derived classes
    py::class_<expression::Constant, expression::Expression, std::shared_ptr<expression::Constant>>(m, "Constant")
        .def(py::init<int>())
        .def("getValue", &expression::Constant::getValue)
        .def("toStr", &expression::Constant::toStr)
        .def("replace", &expression::Constant::replace);

    py::class_<expression::AccessPath, expression::Expression, std::shared_ptr<expression::AccessPath>>(m, "AccessPath")
        .def(py::init<std::string>())
        .def("getName", &expression::AccessPath::getName)
        .def("toStr", &expression::AccessPath::toStr)
        .def("replace", &expression::AccessPath::replace);

    py::class_<expression::Add, expression::Expression, std::shared_ptr<expression::Add>>(m, "Add")
        .def(py::init<std::shared_ptr<expression::Expression>, std::shared_ptr<expression::Expression>>())
        .def("toStr", &expression::Add::toStr)
        .def("replace", &expression::Add::replace);

    py::class_<expression::Sub, expression::Expression, std::shared_ptr<expression::Sub>>(m, "Sub")
        .def(py::init<std::shared_ptr<expression::Expression>, std::shared_ptr<expression::Expression>>())
        .def("toStr", &expression::Sub::toStr)
        .def("replace", &expression::Sub::replace);

    py::class_<expression::Mul, expression::Expression, std::shared_ptr<expression::Mul>>(m, "Mul")
        .def(py::init<std::shared_ptr<expression::Expression>, std::shared_ptr<expression::Expression>>())
        .def("toStr", &expression::Mul::toStr)
        .def("replace", &expression::Mul::replace);
}