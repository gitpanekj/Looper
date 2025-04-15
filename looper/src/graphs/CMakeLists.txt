add_library(graphs directed_labeled_graph.cpp labeled_transition_system.cpp lts_to_dcp_mapper.cpp difference_constraint_program.cpp variable_flow_graph.cpp reset_chain_graph.cpp)
target_include_directories(graphs PUBLIC ${CMAKE_SOURCE_DIR}/src/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/modules/graphs)
pybind11_add_module(PyGraphs py_directed_labeled_graph.cpp)
target_link_libraries(PyGraphs PRIVATE graphs)
target_include_directories(PyGraphs PRIVATE ${CMAKE_SOURCE_DIR}/src/include)
set_target_properties(PyGraphs PROPERTIES
    OUTPUT_NAME "graphs"
    PREFIX ""
    SUFFIX ".cpython-312-x86_64-linux-gnu.so")
