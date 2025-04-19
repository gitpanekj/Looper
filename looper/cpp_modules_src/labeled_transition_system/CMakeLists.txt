add_library(labeled_transition_system labeled_transition_system.cpp)
target_include_directories(labeled_transition_system PUBLIC ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cpp_modules/labeled_transition_system)
file(WRITE "${CMAKE_SOURCE_DIR}/cpp_modules/labeled_transition_system/__init__.py"
     "from labeled_transition_system.labeled_transition_system import *\n")
pybind11_add_module(PyLabeledTransitionSytem py_labeled_transition_system.cpp)
target_link_libraries(PyLabeledTransitionSytem PRIVATE labeled_transition_system directed_labeled_graph)
target_include_directories(PyLabeledTransitionSytem PRIVATE ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set_target_properties(PyLabeledTransitionSytem PROPERTIES
    OUTPUT_NAME "labeled_transition_system"
    PREFIX ""
    SUFFIX ".cpython-312-x86_64-linux-gnu.so")
