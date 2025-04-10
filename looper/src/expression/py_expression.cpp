#include "expression/expression.hpp"
#include "expression/predicate.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <memory>

namespace py = pybind11;

PYBIND11_MODULE(expression, m)
{

    // Expression
    py::class_<Expression, std::shared_ptr<Expression>>(m, "Expression")
        // Factory static methods
        .def_static("create_constant", &Expression::create_constant)
        .def_static("create_variable", &Expression::create_variable)
        .def_static("create_addition", &Expression::create_addition)
        .def_static("create_subtraction", &Expression::create_subtraction)
        .def_static("create_negation", &Expression::create_negation)
        .def_static("create_multiplication", &Expression::create_multiplication)
        // standard methods
        .def("copy", &Expression::copy)
        .def("expand", &Expression::expand)
        .def("substitute", &Expression::substitute)
        .def("rename_variable", &Expression::rename_variable)
        .def("is_constant", &Expression::is_constant)
        .def("separate_constant", &Expression::separate_constant)
        .def("get_variable_names", &Expression::get_variable_names)

        // Operators
        .def("__add__",  [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> std::shared_ptr<Expression> { return a + b; })
        .def("__iadd__", [](std::shared_ptr<Expression>& a,       const std::shared_ptr<Expression>& b) -> std::shared_ptr<Expression> { return a += b; })
        .def("__sub__",  [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> std::shared_ptr<Expression> { return a - b; })
        .def("__isub__", [](std::shared_ptr<Expression>& a,       const std::shared_ptr<Expression>& b) -> std::shared_ptr<Expression> { return a -= b; })
        .def("__mul__",  [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> std::shared_ptr<Expression> { return a * b; })
        .def("__imul__", [](std::shared_ptr<Expression>& a,       const std::shared_ptr<Expression>& b) -> std::shared_ptr<Expression> { return a = b; })
        .def("__neg__",  [](const std::shared_ptr<Expression>& a                                      ) -> std::shared_ptr<Expression> { return -a; })
        .def("__eq__",   [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> bool { return a == b; })
        .def("__ne__",   [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> bool { return a != b; })
        .def("__lt__",   [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> bool { return a <  b; })
        .def("__le__",   [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> bool { return a <= b; })
        .def("__gt__",   [](const std::shared_ptr<Expression>& a, const std::shared_ptr<Expression>& b) -> bool { return a >  b; })
        
        // printing utils
        .def("__repr__", &Expression::to_string)
        .def("to_hash", &Expression::to_hash);

    // Predicate
    py::class_<Predicate, std::shared_ptr<Predicate>>(m, "Predicate")
        .def("getRHS", &Predicate::getRHS)
        .def("getLHS", &Predicate::getLHS)
        .def("to_string", &Predicate::to_string)
        .def("negate", &Predicate::negate);

    py::class_<Less, Predicate, std::shared_ptr<Less>>(m, "Less")
        .def(py::init<std::shared_ptr<Expression>, std::shared_ptr<Expression>>());

    py::class_<LessOrEqual, Predicate, std::shared_ptr<LessOrEqual>>(m, "LessOrEqual")
        .def(py::init<std::shared_ptr<Expression>, std::shared_ptr<Expression>>());

    py::class_<Greater, Predicate, std::shared_ptr<Greater>>(m, "Greater")
        .def(py::init<std::shared_ptr<Expression>, std::shared_ptr<Expression>>());

    py::class_<GreaterOrEqual, Predicate, std::shared_ptr<GreaterOrEqual>>(m, "GreaterOrEqual")
        .def(py::init<std::shared_ptr<Expression>, std::shared_ptr<Expression>>());

    py::class_<Equal, Predicate, std::shared_ptr<Equal>>(m, "Equal")
        .def(py::init<std::shared_ptr<Expression>, std::shared_ptr<Expression>>());

    py::class_<NotEqual, Predicate, std::shared_ptr<NotEqual>>(m, "NotEqual")
        .def(py::init<std::shared_ptr<Expression>, std::shared_ptr<Expression>>());
}