cmake_minimum_required(VERSION 2.8.12)

project(mini_vs_micro_cdr_performance_test C)
set(CMAKE_BUILD_TYPE Release)

include_directories(../../include)
file(GLOB TEST_SOURCES "../../src/c/mini_cdr.c" "mini_vs_micro_cdr_performance_test.c")
add_executable(mini_vs_micro_cdr_performance_test ${TEST_SOURCES})

#find_library(${CMAKE_PREFIX_PATH}/microcdr)
target_link_libraries(mini_vs_micro_cdr_performance_test microcdr)
