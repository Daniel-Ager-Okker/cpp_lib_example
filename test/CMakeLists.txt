cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(test-employee-lib CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

find_package(GTest REQUIRED)
if (${GTest_FOUND})
    message(STATUS "Found GTest framework for testing C++ code")
endif()

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    employee-lib

    gtest
    gtest_main
)

target_include_directories(${PROJECT_NAME} PRIVATE
    GTest/include # google test framework
)

set(USE_ASAN true)
if (${USE_ASAN})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -fsanitize=undefined -fsanitize=address")
    target_link_options(${PROJECT_NAME} BEFORE PUBLIC -fsanitize=undefined PUBLIC -fsanitize=address)
endif()

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})