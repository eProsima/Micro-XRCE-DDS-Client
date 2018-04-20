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
# CMake build rules for micro RTPS client
###############################################################################
cmake_minimum_required(VERSION 2.8.12)
if (${CMAKE_VERSION} VERSION_GREATER 3.0)
	cmake_policy(SET CMP0048 OLD)
endif()

# Set CMAKE_BUILD_TYPE to Release by default.
if(DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
else()
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    )
endif()

###############################################################################
# Product information                                                         #
###############################################################################
project(micrortps-client C CXX)

# Set product name
if(DEFINED PRODUCT_NAME)
    set(PRODUCT_NAME ${PRODUCT_NAME} CACHE STRING "Choose the product name in which this project is included")
else()
    set(PRODUCT_NAME micrortps CACHE STRING "Choose the product name in which this project is included")
endif()

###############################################################################
# Version information                                                         #
###############################################################################
set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 3)
set(PROJECT_MICRO_VERSION 0)
set(PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_MICRO_VERSION})
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

###############################################################################
# eProsima build options
###############################################################################
# Project specific options
set(MAX_MESSAGE_SIZE 512 CACHE STRING "Define maximum message size to be used for messages")
set(DATA_CALLBACK_SIZE 10 CACHE STRING "Define data callback size")

#option(EPROSIMA_VERBOSE "Compile examples" OFF)
option(EPROSIMA_BUILD_EXAMPLES "Compile examples" OFF)
option(EPROSIMA_BUILD_TESTS "Activate the building tests" OFF)
option(EPROSIMA_BUILD "Activate internal building" OFF)
option(THIRDPARTY "Activate the build of thirdparties" OFF)
option(UPDATE_SUBMODULES "Update to the latest" OFF)

if(EPROSIMA_BUILD)
    set(THIRDPARTY ON)
endif()

###############################################################################
# Check MSVC architecture
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/check_configuration.cmake)
if(MSVC OR MSVC_IDE)
    check_msvc_arch()
endif()

###############################################################################
# Installation paths
###############################################################################
set(BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C headers")
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")
if(WIN32)
    set(LICENSE_INSTALL_DIR . CACHE PATH "Installation directory for licenses")
else()
    set(LICENSE_INSTALL_DIR ${DATA_INSTALL_DIR}/${PROJECT_NAME} CACHE PATH "Installation directory for licenses")
endif()

###############################################################################
# Test system configuration
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/check_configuration.cmake)

option(CHECK_ENDIANNESS "Enable/disable endiannes checking" ON)
if(CHECK_ENDIANNESS)
    check_endianness()
else()
    message(STATUS "WARNING!! micrortps-client without check endianness")
endif()

###############################################################################
# Load external eProsima projects.
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/eprosima_libraries.cmake)
if(NOT TARGET transport)
  add_subdirectory(thirdparty/micrortps_transport)
endif()
add_subdirectory(thirdparty/micro-CDR)

file(GLOB CLIENT_LIB_SOURCES
    src/c/*.c
    src/c/log/*.c
)

# Flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSERIALIZATION_LOGS -DMESSAGE_LOGS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")

# Warning level
if(MSVC OR MSVC_IDE)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

###############################################################################
# Config
###############################################################################
configure_file(${PROJECT_SOURCE_DIR}/include/${PRODUCT_NAME}/client/config.h.in
    ${PROJECT_BINARY_DIR}/include/${PRODUCT_NAME}/client/config.h
)

add_library(${PROJECT_NAME} STATIC ${CLIENT_LIB_SOURCES})
target_link_libraries(${PROJECT_NAME} PUBLIC microcdr transport)
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/c>
    $<INSTALL_INTERFACE:include>
)

###############################################################################
# Testing
###############################################################################
if(EPROSIMA_BUILD_TESTS AND NOT EPROSIMA_INSTALLER)
    include(${PROJECT_SOURCE_DIR}/cmake/dev/gtest.cmake)
    check_gtest()
    if(GTEST_FOUND)
        enable_testing()
        include(CTest)
        add_subdirectory(test)
    endif()
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

# Install public headers
include(${PROJECT_SOURCE_DIR}/cmake/dev/set_sources.cmake)
set_public_headers_directory(${PROJECT_SOURCE_DIR}/include ${PRODUCT_NAME}
    DESTINATION ${INCLUDE_INSTALL_DIR}
    COMPONENT headers
)

# Install config.h header
set_public_header(${PROJECT_BINARY_DIR}/include ${PRODUCT_NAME}/client/ config.h
    DESTINATION ${INCLUDE_INSTALL_DIR}/${PRODUCT_NAME}/client
    COMPONENT headers
    INSTALL
)

# Install examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/
    DESTINATION examples/${PRODUCT_NAME}/client
    COMPONENT examples
    PATTERN "examples/*/CMakeLists.txt" EXCLUDE
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT libraries
)

install(EXPORT ${PROJECT_NAME}Targets
    DESTINATION ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
    COMPONENT cmake
)

include(${PROJECT_SOURCE_DIR}/cmake/packaging/eProsimaPackaging.cmake)
