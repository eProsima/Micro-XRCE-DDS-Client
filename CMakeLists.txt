cmake_minimum_required(VERSION 2.8.12)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(COMPILE_EXAMPLES "Compile examples" OFF)
option(EPROSIMA_BUILD_TESTS "Activate the building tests" OFF)


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


if(COMPILE_EXAMPLES)
    # Required libs for examples
    add_subdirectory(thirdparty/micro-CDR)
    add_subdirectory(thirdparty/transport)

    # Examples
    add_subdirectory(examples/mini_cdr)
    add_subdirectory(examples/mini_vs_micro_cdr_performance)
    add_subdirectory(examples/message)

    # prototype
    add_subdirectory(examples/prototype/client)
    add_subdirectory(examples/prototype/agent)

endif()


if(EPROSIMA_BUILD_TESTS)
    add_subdirectory(test)
endif()


# For working in 32 bits
#set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
#if(COMPILE_EXAMPLES)
#    set_target_properties(transport mini_cdr_example message_example
#                          prototype_client prototype_agent
#                          PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
#endif()
#if(EPROSIMA_BUILD_TESTS)
#    set_target_properties(mini_cdr_test
#                          PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
#endif()