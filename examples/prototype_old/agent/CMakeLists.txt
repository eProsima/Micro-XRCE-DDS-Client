cmake_minimum_required(VERSION 2.8.12)

project(prototype_agent C)


file(GLOB TEST_SOURCES
    ${PROJECT_NAME}.c
    ../common/*.c
)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} micrortps-client transport)