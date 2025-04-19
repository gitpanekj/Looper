add_library(expression expression.cpp predicate.cpp multiplication_ast_node.cpp addition_ast_node.cpp max_ast_node.cpp min_ast_node.cpp)
target_include_directories(expression PUBLIC ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cpp_modules/expression)
file(WRITE "${CMAKE_SOURCE_DIR}/cpp_modules/expression/__init__.py"
     "from expression.expression import *\n")
pybind11_add_module(PyExpression py_expression.cpp)
target_link_libraries(PyExpression PRIVATE expression)
target_include_directories(PyExpression PRIVATE ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set_target_properties(PyExpression PROPERTIES
    OUTPUT_NAME "expression"
    PREFIX ""
    SUFFIX ".cpython-312-x86_64-linux-gnu.so")