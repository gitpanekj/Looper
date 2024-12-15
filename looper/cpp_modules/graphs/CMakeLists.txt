add_library(graphs directed_labeled_graph.cpp labeled_transition_system.cpp)
target_include_directories(graphs PUBLIC ${CMAKE_SOURCE_DIR}/cpp_modules/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bindings)
pybind11_add_module(PyGraphs py_directed_labeled_graph.cpp)
target_link_libraries(PyGraphs PRIVATE graphs)
target_include_directories(PyGraphs PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(PyGraphs PROPERTIES
    OUTPUT_NAME "graphs"
    PREFIX ""
    SUFFIX ".cpython-312-x86_64-linux-gnu.so")
