# Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
option(VERBOSE "Use verbose output" OFF)

if(EPROSIMA_BUILD)
    set(EPROSIMA_BUILD_EXAMPLES ON)
    set(EPROSIMA_BUILD_TESTS ON)
    set(THIRDPARTY ON)
endif()

# Profiles options
option(PROFILE_CREATE_ENTITIES_BIN "Activate CreateEntitiesBIN profile" OFF) 
option(PROFILE_CREATE_ENTITIES_REF "Activate CreateEntitiesREF profile" OFF) 
option(PROFILE_CREATE_ENTITIES_XML "Activate CreateEntitiesXML profile" OFF) 
option(PROFILE_UDP_TRANSPORT "Activate UDPTransport profile" OFF) 
option(PROFILE_TCP_TRANSPORT "Activate TCPTransport profile" OFF) 
option(PROFILE_UART_TRANSPORT "Activate UARTTransport profile" OFF) 
option(PROFILE_DISCOVERY_ACCESS "Activate DiscoveryAccess profile" OFF)
option(PROFILE_READ_ACCESS "Activate ReadAccess profile" OFF)
option(PROFILE_STATUS_ANSWER "Activate StatusAnswer profile" OFF)
if(PROFILE_CREATE_ENTITIES_BIN OR PROFILE_CREATE_ENTITIES_REF OR PROFILE_CREATE_ENTITIES_XML)
    set(PROFILE_DELETE_ENTITIES ON)
else()
    set(PROFILE_DELETE_ENTITIES OFF)
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
            set(UDP_SRCS src/c/profile/communication/${TT}/${TT}_transport_linux.c)
        elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
            set(UDP_SRCS src/c/profile/communication/${TT}/${TT}_transport_windows.c)
        elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Generic")
            if(${PLATFORM_NAME} STREQUAL "NuttX")
                set(UDP_SRCS src/c/profile/communication/${TT}/${TT}_transport_nuttx.c)
            elseif(${PLATFORM_NAME} STREQUAL "FreeRTOS")
                set(UDP_SRCS src/c/profile/communication/${TT}/${TT}_transport_freertos.c)
            endif()
        endif()
    endif()
endforeach()

# Other sources
set(SRCS
    ${UDP_SRCS}
    ${TCP_SRCS}
    ${UART_SRCS}
    $<$<BOOL:${PROFILE_DISCOVERY_ACCESS}>:src/c/profile/extension/discovery_access.c>
    $<$<BOOL:${PROFILE_CREATE_ENTITIES_BIN}>:src/c/profile/session/create_entities_bin.c>
    $<$<BOOL:${PROFILE_CREATE_ENTITIES_REF}>:src/c/profile/session/create_entities_ref.c>
    $<$<BOOL:${PROFILE_CREATE_ENTITIES_XML}>:src/c/profile/session/create_entities_xml.c>
    $<$<BOOL:${PROFILE_DELETE_ENTITIES}>:src/c/profile/session/delete_entities.c>
    $<$<BOOL:${PROFILE_READ_ACCESS}>:src/c/profile/session/read_access.c>
    $<$<BOOL:${PROFILE_STATUS_ANSWER}>:src/c/profile/session/status_answer.c>
    src/c/session/input_best_effort_stream.c
    src/c/session/input_reliable_stream.c
    src/c/session/output_best_effort_stream.c
    src/c/session/session.c
    src/c/session/session_info.c
    src/c/session/stream_id.c
    src/c/session/stream_storage.c
    src/c/xrce_protocol_serialization.c
    src/c/log/message.c
    )

###############################################################################
# Target
###############################################################################

add_library(${PROJECT_NAME} ${SRCS})
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    C_STANDARD 99
    C_STANDARD_REQUIRED YES
    )
target_link_libraries(${PROJECT_NAME} PUBLIC microcdr micrortps_transport)
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

# Project options
set(_MICRORTPS_MTU_SIZE_ 512 CACHE STRING "Define maximum transmission unit size to be used")
set(_MICRORTPS_RELIABLE_HISTORY_ 16 CACHE STRING "Define maximum transmission unit size to be used")
set(_MICRORTPS_TIMEOUT_MS_ 1 CACHE STRING "Define maximum transmission unit size to be used")
set(_MICRORTPS_MAX_ATTEMPTS_ 20 CACHE STRING "Define maximum transmission unit size to be used")
set(_MICRORTPS_CONNECTION_MAX_ATTEMPTS_ 20 CACHE STRING "Define maximum transmission unit size to be used")
set(_MICRORTPS_HEARTBEAT_MIN_PERIOD_MS_ 200 CACHE STRING "Define maximum transmission unit size to be used")

# Create source file with the define
configure_file(${PROJECT_SOURCE_DIR}/include/micrortps/client/config.h.in
    ${PROJECT_BINARY_DIR}/include/micrortps/client/config.h
)

###############################################################################
# Compile setting
###############################################################################

# Definitions
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        -D$<$<BOOL:${VERBOSE}>:SERIALIZATION_LOGS>
        -D$<$<BOOL:${VERBOSE}>:MESSAGE_LOGS>
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
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -fstrict-aliasing -Wall -Wextra -Wcast-align -Wshadow")
endif()

###############################################################################
# Testing
###############################################################################
if(EPROSIMA_BUILD_TESTS)
    # Static library only for test purposes
    add_library(${PROJECT_NAME}_static STATIC ${SRCS})
    target_link_libraries(${PROJECT_NAME}_static PUBLIC microcdr micrortps_transport)
    target_include_directories(${PROJECT_NAME}_static
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/c>
        )

    # Enable test
    include(${PROJECT_SOURCE_DIR}/cmake/common/gtest.cmake)
    enable_testing()
    add_subdirectory(test)
endif()

###############################################################################
# Examples
###############################################################################
if(EPROSIMA_BUILD_EXAMPLES)
    add_subdirectory(examples/ShapesDemo)
    add_subdirectory(examples/PublishHelloWorld)
    add_subdirectory(examples/SubscribeHelloWorld)
    add_subdirectory(examples/PublishSubscribeHelloWorld)
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
