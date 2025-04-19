add_library(variable_flow_graph variable_flow_graph.cpp)
target_include_directories(variable_flow_graph PUBLIC ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cpp_modules/variable_flow_graph)
file(WRITE "${CMAKE_SOURCE_DIR}/cpp_modules/variable_flow_graph/__init__.py"
     "from variable_flow_graph.variable_flow_graph import *\n")
pybind11_add_module(PyVariableFlowGraph py_variable_flow_graph.cpp)
target_link_libraries(PyVariableFlowGraph PRIVATE variable_flow_graph directed_labeled_graph)
target_include_directories(PyVariableFlowGraph PRIVATE ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set_target_properties(PyVariableFlowGraph PROPERTIES
    OUTPUT_NAME "variable_flow_graph"
    PREFIX ""
    SUFFIX ".cpython-312-x86_64-linux-gnu.so")
