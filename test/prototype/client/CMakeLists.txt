cmake_minimum_required(VERSION 2.8.12)

project(prototype_client C)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB TEST_SOURCES "prototype_client.c" "../shape_topic.c")
add_executable(prototype_client ${TEST_SOURCES})
set_target_properties(prototype_client PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_link_libraries(prototype_client micrortps-client)