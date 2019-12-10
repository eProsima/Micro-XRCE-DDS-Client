# Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

################################################################################
## CMake build rules for Micro XRCE-DDS Client
################################################################################
cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

###############################################################################
# Build options
###############################################################################
option(UCLIENT_SUPERBUILD "Enable superbuild compilation." ON)
option(UCLIENT_BUILD_TESTS "Build tests." OFF)
option(UCLIENT_BUILD_EXAMPLES "Build examples." OFF)
option(UCLIENT_VERBOSE_SERIALIZATION "Build with serialization verbosity." OFF)
option(UCLIENT_VERBOSE_MESSAGE "Build with message verbosity." OFF)
option(UCLIENT_PIC "Control Position Independent Code." ON)
option(BUILD_SHARED_LIBS "Control shared/static library building." OFF)

option(UCLIENT_BUILD_CI_TESTS "Build CI test cases." OFF)
if(UCLIENT_BUILD_CI_TESTS)
    set(UCLIENT_BUILD_TESTS ON)
    set(UCLIENT_BUILD_EXAMPLES ON)
endif()

if((CMAKE_SYSTEM_NAME STREQUAL "") AND (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux"))
    option(UCLIENT_MEMORY_TESTS "Build memory tests." OFF)
    option(UCLIENT_PERFORMANCE_TESTS "Build performance tests." OFF)
endif()

option(UCLIENT_BIG_ENDIANNESS "Set the machine endianness to big endianness (by default is little endianness)." OFF)

set(UCLIENT_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/client.config" CACHE PATH "Configuration client file.")

include(GNUInstallDirs)
set(BIN_INSTALL_DIR     ${CMAKE_INSTALL_BINDIR}     CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for C headers")
set(LIB_INSTALL_DIR     ${CMAKE_INSTALL_LIBDIR}     CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR    ${CMAKE_INSTALL_DATADIR}    CACHE PATH "Installation directory for data")
if(WIN32)
    set(LICENSE_INSTALL_DIR . CACHE PATH "Installation directory for licenses")
else()
    set(LICENSE_INSTALL_DIR ${DATA_INSTALL_DIR}/${PROJECT_NAME} CACHE PATH "Installation directory for licenses")
endif()

###############################################################################
# Dependencies
###############################################################################
set(_microcdr_version 1.1.1)
set(_microcdr_tag v1.1.1)

set(_deps "")
list(APPEND _deps "microcdr\;${_microcdr_version}")

###############################################################################
# Project
###############################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
if(NOT UCLIENT_SUPERBUILD)
    project(microxrcedds_client VERSION "1.1.5" LANGUAGES C)
else()
    project(uclient_superbuild NONE)
    include(${PROJECT_SOURCE_DIR}/cmake/SuperBuild.cmake)
    return()
endif()

# Set CMAKE_BUILD_TYPE to Release by default.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

###############################################################################
# Client configuration options
###############################################################################
# Load configuration file.
if(EXISTS ${UCLIENT_CONFIG})
    configure_file(${UCLIENT_CONFIG} ${PROJECT_SOURCE_DIR}/CMakeCache.txt COPYONLY)
    load_cache(${PROJECT_SOURCE_DIR})
    file(REMOVE ${PROJECT_SOURCE_DIR}/CMakeCache.txt)
else()
    message(FATAL_ERROR "No client.config found.")
endif()

###############################################################################
# Check MSVC architecture
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/common/check_configuration.cmake)
if(MSVC OR MSVC_IDE)
    check_msvc_arch()
endif()

###############################################################################
# Load external eProsima projects.
###############################################################################
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_BINARY_DIR}/temp_install)

foreach(d ${_deps})
    list(GET d 0 _name)
    list(GET d 1 _version)
    find_package(${_name} ${_version} EXACT REQUIRED)
endforeach()

###############################################################################
# Sources
###############################################################################
# Check platform.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_NAME_LINUX ON)
    set(TRANSPORT_TYPES "UDP" "TCP" "SERIAL")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_NAME_WINDOWS ON)
    set(TRANSPORT_TYPES "UDP" "TCP")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Generic")
    if(PLATFORM_NAME STREQUAL "nuttx")
        set(PLATFORM_NAME_NUTTX ON)
        set(TRANSPORT_TYPES "UDP" "TCP" "SERIAL")
    endif()
endif()

# Transport sources.
foreach(TRANS_TYPE ${TRANSPORT_TYPES})
    if(PROFILE_${TRANS_TYPE}_TRANSPORT)
        string(TOLOWER ${TRANS_TYPE} TT)
        if(PLATFORM_NAME_LINUX)
            set(${TRANS_TYPE}_SRCS
                src/c/profile/transport/${TT}/${TT}_transport.c
                src/c/profile/transport/${TT}/${TT}_transport_linux.c
                )
        elseif(PLATFORM_NAME_WINDOWS)
            set(${TRANS_TYPE}_SRCS
                src/c/profile/transport/${TT}/${TT}_transport.c
                src/c/profile/transport/${TT}/${TT}_transport_windows.c
                )
        elseif(PLATFORM_NAME_NUTTX)
            set(${TRANS_TYPE}_SRCS
                src/c/profile/transport/${TT}/${TT}_transport.c
                src/c/profile/transport/${TT}/${TT}_transport_linux.c
                )
        endif()
    endif()
endforeach()

if(PROFILE_SERIAL_TRANSPORT)
    set(SERIAL_SRCS ${SERIAL_SRCS} src/c/profile/transport/serial/serial_protocol.c)
endif()

# Transport discovery source.
if(PROFILE_DISCOVERY)
    if(PLATFORM_NAME_LINUX)
        set(UDP_DISCOVERY_SRCS src/c/profile/discovery/transport/udp_transport_datagram_linux.c)
    elseif(PLATFORM_NAME_WINDOWS)
        set(UDP_DISCOVERY_SRCS src/c/profile/discovery/transport/udp_transport_datagram_windows.c)
    elseif(PLATFORM_NAME_NUTTX)
        set(UDP_DISCOVERY_SRCS src/c/profile/discovery/transport/udp_transport_datagram_linux.c)
    endif()
endif()

# Other sources
set(SRCS
    src/c/core/session/stream/input_best_effort_stream.c
    src/c/core/session/stream/input_reliable_stream.c
    src/c/core/session/stream/output_best_effort_stream.c
    src/c/core/session/stream/output_reliable_stream.c
    src/c/core/session/stream/stream_storage.c
    src/c/core/session/stream/stream_id.c
    src/c/core/session/stream/seq_num.c
    src/c/core/session/session.c
    src/c/core/session/session_info.c
    src/c/core/session/submessage.c
    src/c/core/session/object_id.c
    src/c/core/serialization/xrce_protocol.c
    src/c/core/serialization/xrce_header.c
    src/c/core/serialization/xrce_subheader.c
    src/c/util/time.c
    src/c/core/session/common_create_entities.c
    src/c/core/session/create_entities_ref.c
    src/c/core/session/create_entities_xml.c
    src/c/core/session/read_access.c
    src/c/core/session/write_access.c
    $<$<OR:$<BOOL:${UCLIENT_VERBOSE_MESSAGE}>,$<BOOL:${UCLIENT_VERBOSE_SERIALIZATION}>>:src/c/core/log/log.c>
    $<$<BOOL:${PROFILE_DISCOVERY}>:src/c/profile/discovery/discovery.c>
    ${UDP_DISCOVERY_SRCS}
    ${UDP_SRCS}
    ${TCP_SRCS}
    ${SERIAL_SRCS}
    )

###############################################################################
# Targets
###############################################################################
# Library
add_library(${PROJECT_NAME} ${SRCS})

set_common_compile_options(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION
        ${PROJECT_VERSION}
    SOVERSION
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    C_STANDARD
        99
    C_STANDARD_REQUIRED
        YES
    POSITION_INDEPENDENT_CODE
	    ${UCLIENT_PIC}
    )

target_compile_options(${PROJECT_NAME}
    PRIVATE
        $<$<C_COMPILER_ID:GNU>:-fdata-sections -ffunction-sections>
    )

# Set Windows shared libraries name
get_target_property(UCLIENT_TARGET_TYPE ${PROJECT_NAME} TYPE)
if((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND (UCLIENT_TARGET_TYPE STREQUAL "SHARED_LIBRARY"))
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RELEASE_POSTFIX
            -${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        RELWITHDEBINFO_POSTFIX
            -${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        DEBUG_POSTFIX
            d-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        )

    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            ${PROJECT_NAME}_SHARED
        )
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        microcdr
        $<$<BOOL:$<PLATFORM_ID:Windows>>:ws2_32>
    PRIVATE
        $<$<BOOL:$<PLATFORM_ID:Linux>>:rt>
    )
target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/c>
    )

###############################################################################
# Compile setting
###############################################################################
# Definitions
target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        $<$<BOOL:${UCLIENT_PERFORMANCE_TESTS}>:PERFORMANCE_TESTING>
    PRIVATE
        $<$<BOOL:${UCLIENT_VERBOSE_SERIALIZATION}>:UXR_SERIALIZATION_LOGS>
        $<$<BOOL:${UCLIENT_VERBOSE_MESSAGE}>:UXR_MESSAGE_LOGS>
    )

get_target_property(TARGET_TYPE ${PROJECT_NAME} TYPE)
if(MSVC OR MSVC_IDE)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4996)
    if(TARGET_TYPE STREQUAL "SHARED_LIBRARY")
        target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            ${PROJECT_NAME}_SHARED
        )
    endif()
endif()

###############################################################################
# Examples
###############################################################################
if(UCLIENT_BUILD_EXAMPLES)
    add_subdirectory(examples/PublishHelloWorld)
    add_subdirectory(examples/SubscribeHelloWorld)
    add_subdirectory(examples/PublishHelloWorldP2P)
    add_subdirectory(examples/SubscribeHelloWorldP2P)
    add_subdirectory(examples/ShapesDemo)
    add_subdirectory(examples/Deployment)
    add_subdirectory(examples/Discovery)
endif()

###############################################################################
# Tests
###############################################################################
if(UCLIENT_BUILD_CI_TESTS)
    include(CTest)
    add_subdirectory(test/case)
endif()

if(UCLIENT_BUILD_TESTS)
    include(${PROJECT_SOURCE_DIR}/cmake/common/gtest.cmake)
    find_package(GTest REQUIRED)
    find_package(GMock REQUIRED)
    find_package(Threads REQUIRED)
    enable_testing()
    include(CTest)

    add_subdirectory(test/unitary)
    if(PLATFORM_NAME_LINUX)
        add_subdirectory(test/transport/serial_comm)
    endif()
endif()

if(PLATFORM_NAME_LINUX AND UCLIENT_MEMORY_TESTS)
    add_subdirectory(test/memory/consumption)
endif()

###############################################################################
# Packaging
###############################################################################
# Install library
install(
    TARGETS
        ${PROJECT_NAME}
    EXPORT
        ${PROJECT_NAME}Targets
    RUNTIME DESTINATION
        ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION
        ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION
        ${LIB_INSTALL_DIR}
    COMPONENT libraries
    )

# Install includes
install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}/include/uxr/client/
    DESTINATION
        ${INCLUDE_INSTALL_DIR}/uxr/client
    FILES_MATCHING
        PATTERN "*.h"
    )

# Generate config.h
configure_file(${PROJECT_SOURCE_DIR}/include/uxr/client/config.h.in
    ${PROJECT_BINARY_DIR}/include/uxr/client/config.h
    )

# Install config.h
install(
    FILES
        ${PROJECT_BINARY_DIR}/include/uxr/client/config.h
    DESTINATION
        ${INCLUDE_INSTALL_DIR}/uxr/client
    )

# Export library
install(
    EXPORT
        ${PROJECT_NAME}Targets
    DESTINATION
        ${DATA_INSTALL_DIR}/${PROJECT_NAME}/cmake
    )

# Package configuration
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/packaging/Config.cmake.in
        ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION
        ${DATA_INSTALL_DIR}/${PROJECT_NAME}/cmake
    PATH_VARS
        BIN_INSTALL_DIR
        INCLUDE_INSTALL_DIR
        LIB_INSTALL_DIR
        DATA_INSTALL_DIR
    )

write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}ConfigVersion.cmake
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        SameMajorVersion
    )

install(
    FILES
        ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Config.cmake
        ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION
        ${DATA_INSTALL_DIR}/${PROJECT_NAME}/cmake
    )

# Install dependencies
if(EXISTS ${CMAKE_BINARY_DIR}/temp_install/)
    install(
        DIRECTORY
            ${CMAKE_BINARY_DIR}/temp_install/
        DESTINATION
            ${CMAKE_INSTALL_PREFIX}
        )
endif()
