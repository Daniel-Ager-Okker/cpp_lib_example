cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(employee-factory-lib CXX)
set(CMAKE_CXX_STANDARD 17)

enable_testing()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

add_subdirectory(src)
add_subdirectory(test)