cmake_minimum_required(VERSION 2.8.12)

project(mini_vs_micro_cdr_performance C)

include_directories(../../include)

file(GLOB TEST_SOURCES
    ../../src/c/mini_cdr.c
    ${PROJECT_NAME}.c
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} microcdr)
