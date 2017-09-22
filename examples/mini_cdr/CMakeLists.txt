cmake_minimum_required(VERSION 2.8.12)

project(mini_cdr_example C)

file(GLOB TEST_SOURCES
    ${PROJECT_NAME}.c
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} micrortps-client)