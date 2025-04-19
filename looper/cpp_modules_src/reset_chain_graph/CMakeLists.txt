add_library(reset_chain_graph reset_chain_graph.cpp)
target_include_directories(reset_chain_graph PUBLIC ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cpp_modules/reset_chain_graph)
file(WRITE "${CMAKE_SOURCE_DIR}/cpp_modules/reset_chain_graph/__init__.py"
     "from reset_chain_graph.reset_chain_graph import *\n")
pybind11_add_module(PyResetChainGraph py_reset_chain_graph.cpp)
target_link_libraries(PyResetChainGraph PRIVATE reset_chain_graph directed_labeled_graph)
target_include_directories(PyResetChainGraph PRIVATE ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set_target_properties(PyResetChainGraph PROPERTIES
    OUTPUT_NAME "reset_chain_graph"
    PREFIX ""
    SUFFIX ".cpython-312-x86_64-linux-gnu.so")
