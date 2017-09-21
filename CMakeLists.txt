cmake_minimum_required(VERSION 2.8.12)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(micrortps-client C)


file(GLOB CLIENT_LIB_SOURCES
    src/c/*.c
    src/c/*/*.c
)

add_library(${PROJECT_NAME} SHARED ${CLIENT_LIB_SOURCES})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

option(EPROSIMA_BUILD_TESTS "Activate the building tests" OFF)
if(EPROSIMA_BUILD_TESTS)
    # Required libs for testing
    add_subdirectory(thirdparty/micro-CDR)
    add_subdirectory(thirdparty/transport)

    # Tests
    add_subdirectory(test/message)
    add_subdirectory(test/mini_cdr)
    add_subdirectory(test/mini_vs_micro_cdr_performance)
    add_subdirectory(test/prototype/client)
    add_subdirectory(test/prototype/agent)
endif()