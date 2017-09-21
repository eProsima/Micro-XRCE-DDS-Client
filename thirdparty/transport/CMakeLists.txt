cmake_minimum_required(VERSION 2.8.12)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(transport C)


file(GLOB CLIENT_LIB_SOURCES
    src/c/*/*.c
)

add_library(transport SHARED ${CLIENT_LIB_SOURCES})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}>
)
