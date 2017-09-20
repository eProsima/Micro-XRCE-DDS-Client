cmake_minimum_required(VERSION 2.8.12)

project(message_test C)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB TEST_SOURCES "message_test.c")
add_executable(message_test ${TEST_SOURCES})
set_target_properties(message_test PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_link_libraries(message_test micrortps-client)