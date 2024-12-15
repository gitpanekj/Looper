add_library(expression expression.cpp)
target_include_directories(expression PUBLIC ${CMAKE_SOURCE_DIR}/cpp_modules/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bindings)
pybind11_add_module(PyExpression py_expression.cpp)
target_link_libraries(PyExpression PRIVATE expression)
target_include_directories(PyExpression PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(PyExpression PROPERTIES
    OUTPUT_NAME "expression"
    PREFIX ""
    SUFFIX ".cpython-312-x86_64-linux-gnu.so")