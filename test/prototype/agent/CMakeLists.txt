cmake_minimum_required(VERSION 2.8.12)

project(prototype_agent C)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB TEST_SOURCES "prototype_agent.c" "../shape_topic.c")
add_executable(prototype_agent ${TEST_SOURCES})
set_target_properties(prototype_agent PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_link_libraries(prototype_agent micrortps-client)