# Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

###############################################################################
# CMake build rules for Micro RTPS Client
###############################################################################
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(IS_TOP_LEVEL TRUE)
if(PROJECT_SOURCE_DIR)
    set(IS_TOP_LEVEL FALSE)
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
# Product information
###############################################################################
if(CMAKE_VERSION VERSION_LESS 3.0)
    project(micrortps_client C)
    set(PROJECT_VERSION_MAJOR 1)
    set(PROJECT_VERSION_MINOR 0)
    set(PROJECT_VERSION_PATCH 0)
    set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
else()
    cmake_policy(SET CMP0048 NEW)
    project(micrortps_client VERSION "1.0.0" LANGUAGES C)
endif()

###############################################################################
# eProsima build options
###############################################################################
option(EPROSIMA_BUILD "Activate internal building" OFF)
option(EPROSIMA_BUILD_EXAMPLES "Compile examples" OFF)
option(EPROSIMA_BUILD_TESTS "Activate the building tests" OFF)
option(THIRDPARTY "Activate the build of thirdparties" OFF)
option(VERBOSE_SERIALIZATION "Use verbose output" OFF)
option(VERBOSE_MESSAGE "Use verbose output" OFF)

if(EPROSIMA_BUILD)
    set(EPROSIMA_BUILD_EXAMPLES ON)
    set(EPROSIMA_BUILD_TESTS ON)
    set(THIRDPARTY ON)
endif()

###############################################################################
# Client configuration options
###############################################################################
file(STRINGS ${PROJECT_SOURCE_DIR}/client.config ConfigContents)
foreach(NameAndValue ${ConfigContents})
  string(REGEX REPLACE "^[ ]+" "" NameAndValue ${NameAndValue})
  string(REGEX MATCH "^[^=]+" Name ${NameAndValue})
  string(REPLACE "${Name}=" "" Value ${NameAndValue})
  set(${Name} "${Value}")
endforeach()

if(PROFILE_CREATE_ENTITIES_REF OR PROFILE_CREATE_ENTITIES_XML)
    set(PROFILE_COMMON_CREATE_ENTITIES ON)
else()
    set(PROFILE_COMMON_CREATE_ENTITIES OFF)
endif()

if(CONFIG_MACHINE_ENDIANNESS)
    set(__BIG_ENDIAN__ CONFIG_MACHINE_ENDIANNESS)
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
include(${PROJECT_SOURCE_DIR}/cmake/common/eprosima_libraries.cmake)
eprosima_find_package(microcdr)

###############################################################################
# Sources
###############################################################################

# Transport source
foreach(TRANSPORT_TYPE UDP TCP UART)
    if(PROFILE_${TRANSPORT_TYPE}_TRANSPORT)
        string(TOLOWER ${TRANSPORT_TYPE} TT)
        if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
            set(PLATFORM_NAME_LINUX ON)
            set(${TRANSPORT_TYPE}_SRCS src/c/profile/transport/${TT}_transport_linux.c)
        elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
            set(PLATFORM_NAME_WINDOWS ON)
            set(${TRANSPORT_TYPE}_SRCS src/c/profile/transport/${TT}_transport_windows.c)
        elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Generic")
            if(${PLATFORM_NAME} STREQUAL "nuttx")
                set(PLATFORM_NAME_NUTTX ON)
                set(${TRANSPORT_TYPE}_SRCS src/c/profile/transport/${TT}_transport_linux.c)
            endif()
        endif()
    endif()
endforeach()

# Other sources
set(SRCS
    $<$<BOOL:${PROFILE_COMMON_CREATE_ENTITIES}>:src/c/profile/session/common_create_entities.c>
    $<$<BOOL:${PROFILE_CREATE_ENTITIES_REF}>:src/c/profile/session/create_entities_ref.c>
    $<$<BOOL:${PROFILE_CREATE_ENTITIES_XML}>:src/c/profile/session/create_entities_xml.c>
    $<$<BOOL:${PROFILE_READ_ACCESS}>:src/c/profile/session/read_access.c>
    ${UDP_SRCS}
    ${TCP_SRCS}
    ${UART_SRCS}
    src/c/core/communication/serial_protocol.c
    src/c/core/session/input_best_effort_stream.c
    src/c/core/session/input_reliable_stream.c
    src/c/core/session/output_best_effort_stream.c
    src/c/core/session/output_reliable_stream.c
    src/c/core/session/session.c
    src/c/core/session/session_info.c
    src/c/core/session/stream_id.c
    src/c/core/session/stream_storage.c
    src/c/core/session/submessage.c
    src/c/core/session/seq_num.c
    src/c/core/session/object_id.c
    src/c/core/serialization/xrce_protocol.c
    src/c/core/serialization/xrce_header.c
    src/c/core/serialization/xrce_subheader.c
    src/c/core/util/time.c
    src/c/core/session/log/message.c
    )

###############################################################################
# Target
###############################################################################
set(BUILD_SHARED_LIBS OFF)
add_library(${PROJECT_NAME} ${SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    )
target_link_libraries(${PROJECT_NAME} PUBLIC microcdr
    $<$<BOOL:${WIN32}>:ws2_32>
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
# Config
###############################################################################

# Install path
set(BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C headers")
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")
if(WIN32)
    set(LICENSE_INSTALL_DIR . CACHE PATH "Installation directory for licenses")
else()
    set(LICENSE_INSTALL_DIR ${DATA_INSTALL_DIR}/${PROJECT_NAME} CACHE PATH "Installation directory for licenses")
endif()

# Create source files with the define
configure_file(${PROJECT_SOURCE_DIR}/include/micrortps/client/config.h.in
    ${PROJECT_BINARY_DIR}/include/micrortps/client/config.h
    )

###############################################################################
# Compile setting
###############################################################################


# Definitions
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        -D$<$<BOOL:${VERBOSE_SERIALIZATION}>:SERIALIZATION_LOGS>
        -D$<$<BOOL:${VERBOSE_MESSAGE}>:MESSAGE_LOGS>
    )

get_target_property(TARGET_TYPE ${PROJECT_NAME} TYPE)
if((MSVC OR MSVC_IDE) AND (TARGET_TYPE STREQUAL "SHARED_LIBRARY"))
    target_compile_definitions(${PROJECT_NAME}
        PUBLIC
            -D${PROJECT_NAME}_SHARED
        )
endif()

# Warnings
if(MSVC OR MSVC_IDE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4 /wd4700 /wd4996 /wd4820 /wd4255 /wd4668")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4700 /wd4996 /wd4820 /wd4255 /wd4668")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -fstrict-aliasing -Wall -Wextra -Wcast-align -Wshadow")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fstrict-aliasing -Wall -Wextra -Wcast-align")
endif()

###############################################################################
# Examples
###############################################################################
if(EPROSIMA_BUILD_EXAMPLES)
    add_subdirectory(examples/PublishHelloWorld)
    add_subdirectory(examples/SubscribeHelloWorld)
    add_subdirectory(examples/ShapesDemo)
endif()

###############################################################################
# Test
###############################################################################
if(EPROSIMA_BUILD_TESTS AND IS_TOP_LEVEL)
    include(${PROJECT_SOURCE_DIR}/cmake/common/gtest.cmake)
    check_gtest()
    enable_testing()
    include(CTest)

    add_subdirectory(test/integration/interaction)
    add_subdirectory(test/integration/cross_serialization)
    add_subdirectory(test/unitary/streams/output_best_effort)

    if(NOT WIN32)
        add_subdirectory(test/transport/serial_comm)
    endif()
endif()

###############################################################################
# Packaging
###############################################################################

# Install library
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT libraries
    )

# Install includes
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/micrortps/client/
    DESTINATION ${INCLUDE_INSTALL_DIR}/micrortps/client
    FILES_MATCHING PATTERN "*.h"
    )

# Install config.h
install(FILES ${PROJECT_BINARY_DIR}/include/micrortps/client/config.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/micrortps/client
    )

# Export library
install(EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
    )

# Package configuration
include(CMakePackageConfigHelpers)
configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/common/Config.cmake.in
    ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}/${PROJECT_NAME}/cmake
    PATH_VARS BIN_INSTALL_DIR INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
    )
install(FILES ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Config.cmake
    DESTINATION ${LIB_INSTALL_DIR}${DIR_EXTENSION}/${PROJECT_NAME}/cmake
    )
