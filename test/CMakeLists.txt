cmake_minimum_required(VERSION 2.8.12)

project(tests)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(mini_cdr_test mini_cdr_test.cpp)
target_link_libraries(mini_cdr_test ${GTEST_LIBRARIES} pthread micrortps-client)