#include "expression/expression.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <memory>
namespace py = pybind11;
using namespace expression;


PYBIND11_MODULE(expression, m) {
    // Abstract classes
    py::class_<Term, std::shared_ptr<Term>>(m, "Term")
        .def("replace", &Term::replace)
        .def("toStr", &Term::toStr)
        .def("deepCopy", &Term::deepCopy);

    py::class_<BinaryOperation, Term, std::shared_ptr<BinaryOperation>>(m, "BinaryOperation")
        .def("toStr", &BinaryOperation::toStr)
        .def("replace", &BinaryOperation::replace)
        .def("deepCopy", &BinaryOperation::deepCopy);

    // Constant
    py::class_<IntConstant, Term, std::shared_ptr<IntConstant>>(m, "IntConstant")
        .def(py::init<int>())
        .def("getValue", &IntConstant::getValue);

    // Variable
    py::class_<AccessPath, Term, std::shared_ptr<AccessPath>>(m, "AccessPath")
        .def(py::init<std::string>())
        .def("getName", &AccessPath::getName);

    // Operators
    py::class_<Add, BinaryOperation, std::shared_ptr<Add>>(m, "Add")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());
    
    py::class_<Sub, BinaryOperation, std::shared_ptr<Sub>>(m, "Sub")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());

    // Predicate
    py::class_<Predicate, std::shared_ptr<Predicate>>(m, "Predicate")
        .def("getRHS", &Predicate::getRHS)
        .def("getLHS", &Predicate::getLHS)
        .def("toStr",  &Predicate::toStr)
        .def("negate", &Predicate::negate);
    
    py::class_<Less, Predicate, std::shared_ptr<Less>>(m, "Less")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());

    py::class_<LessOrEqual, Predicate, std::shared_ptr<LessOrEqual>>(m, "LessOrEqual")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());

    py::class_<Greater, Predicate, std::shared_ptr<Greater>>(m, "Greater")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());

    py::class_<GreaterOrEqual, Predicate, std::shared_ptr<GreaterOrEqual>>(m, "GreaterOrEqual")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());

    py::class_<Equal, Predicate, std::shared_ptr<Equal>>(m, "Equal")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());
    
    py::class_<NotEqual, Predicate, std::shared_ptr<NotEqual>>(m, "NotEqual")
        .def(py::init<std::shared_ptr<Term>, std::shared_ptr<Term>>());
}