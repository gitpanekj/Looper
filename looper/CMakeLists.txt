cmake_minimum_required(VERSION 3.20.0)
project(looper)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(cpp_modules)
add_subdirectory(tests)
