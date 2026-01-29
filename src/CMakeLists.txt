cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(employee-lib CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

add_library(${PROJECT_NAME} SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/Employee.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/EmployeeManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Foreman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/RelationManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SalaryCalculator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Worker.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    -static-libstdc++
    -static-libgcc
)

# Public headers
target_sources(${PROJECT_NAME} PUBLIC
    FILE_SET lib_grpc_api_HEADERS
    TYPE HEADERS
    BASE_DIRS ../include/
    FILES
        ../include/employee_lib/EmployeeManager.h
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
)
