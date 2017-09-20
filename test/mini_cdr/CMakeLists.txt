cmake_minimum_required(VERSION 2.8.12)

project(mini_cdr_test C)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB TEST_SOURCES "mini_cdr_test.c")
add_executable(mini_cdr_test ${TEST_SOURCES})
set_target_properties(mini_cdr_test PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_link_libraries(mini_cdr_test micrortps-client)