add_library(difference_constraint_program lts_to_dcp_mapper.cpp difference_constraint_program.cpp)
target_include_directories(difference_constraint_program PUBLIC ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/cpp_modules/difference_constraint_program)
file(WRITE "${CMAKE_SOURCE_DIR}/cpp_modules/difference_constraint_program/__init__.py"
     "from difference_constraint_program.difference_constraint_program import *\n")
pybind11_add_module(PyDifferenceConstraintProgram py_difference_constraint_program.cpp)
target_link_libraries(PyDifferenceConstraintProgram PRIVATE difference_constraint_program directed_labeled_graph labeled_transition_system)
target_include_directories(PyDifferenceConstraintProgram PRIVATE ${CMAKE_SOURCE_DIR}/cpp_modules_src/include)
set_target_properties(PyDifferenceConstraintProgram PROPERTIES
    OUTPUT_NAME "difference_constraint_program"
    PREFIX ""
    SUFFIX "${PYTHON_EXTENSION_SUFFIX}")
