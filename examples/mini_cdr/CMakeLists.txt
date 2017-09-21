cmake_minimum_required(VERSION 2.8.12)

project(mini_cdr_example C)

file(GLOB TEST_SOURCES
    ${PROJECT_NAME}.c
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
target_link_libraries(${PROJECT_NAME} micrortps-client)