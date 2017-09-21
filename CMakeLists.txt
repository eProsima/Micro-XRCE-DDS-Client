cmake_minimum_required(VERSION 2.8.12)

project(micrortps_client C)
set(CMAKE_BUILD_TYPE Debug)

include_directories(include)
file(GLOB CLIENT_LIB_SOURCES "src/c/*.c" "src/c/*/*.c")
add_library(micrortps-client SHARED ${CLIENT_LIB_SOURCES})
set_target_properties(micrortps-client PROPERTIES COMPILE_FLAGS "-D _DEBUG_ -D _VERBOSE_ -m32" LINK_FLAGS "-m32")

option(EPROSIMA_BUILD_TESTS "Activate the building tests" OFF)
if(EPROSIMA_BUILD_TESTS)
    # Required libs for testing
    set(CMAKE_BUILD_TYPE Release)
    add_subdirectory(thirdparty/micro-CDR)

    # Tests
    add_subdirectory(test/message)
    add_subdirectory(test/mini_cdr)
    add_subdirectory(test/mini_vs_micro_cdr_performance)
    add_subdirectory(test/prototype/client)
    add_subdirectory(test/prototype/agent)
endif()