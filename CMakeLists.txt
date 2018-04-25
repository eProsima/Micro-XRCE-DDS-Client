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
# Product information                                                         #
###############################################################################
if(CMAKE_VERSION VERSION_LESS 3.0)
    project(micrortps-client C)
    set(PROJECT_VERSION_MAJOR 1)
    set(PROJECT_VERSION_MINOR 0)
    set(PROJECT_VERSION_PATCH 0)
    set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
else()
    cmake_policy(SET CMP0048 NEW)
    project(micrortps-client VERSION "1.0.0" LANGUAGES C CXX)
endif()

# Set product name
if(DEFINED PRODUCT_NAME)
    set(PRODUCT_NAME ${PRODUCT_NAME} CACHE STRING "Choose the product name in which this project is included")
else()
    set(PRODUCT_NAME micrortps CACHE STRING "Choose the product name in which this project is included")
endif()

message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

###############################################################################
# eProsima build options
###############################################################################
option(EPROSIMA_BUILD_EXAMPLES "Compile examples" OFF)
option(EPROSIMA_BUILD_TESTS "Activate the building tests" OFF)
option(EPROSIMA_BUILD "Activate internal building" OFF)
option(THIRDPARTY "Activate the build of thirdparties" ON)
option(UPDATE_SUBMODULES "Update to the latest" OFF)

if(EPROSIMA_BUILD)
    set(THIRDPARTY ON)
    set(EPROSIMA_BUILD_EXAMPLES ON)
    set(EPROSIMA_BUILD_TESTS ON)
endif()

###############################################################################
# Check MSVC architecture
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/check_configuration.cmake)
if(MSVC OR MSVC_IDE)
    check_msvc_arch()
endif()

###############################################################################
# Load external eProsima projects.
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/eprosima_libraries.cmake)
find_eprosima_package(micrortps_transport)
find_eprosima_package(micro-CDR)

###############################################################################
# Targets
###############################################################################

# Set source files
set(SRCS 
    src/c/client.c
    src/c/xrce_protocol_serialization.c
    src/c/reliable_control.c
    src/c/input_message.c
    src/c/output_message.c
    src/c/log/message.c
    )

# Library
add_library(${PROJECT_NAME} STATIC ${SRCS})
target_link_libraries(${PROJECT_NAME} PUBLIC microcdr transport)
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

# Endianness configuration
include(${PROJECT_SOURCE_DIR}/cmake/dev/check_configuration.cmake)

option(CHECK_ENDIANNESS "Enable/disable endiannes checking" ON)
if(CHECK_ENDIANNESS)
    check_endianness()
else()
    message(STATUS "WARNING!! micrortps-client without check endianness")
endif()

# Create source file with the define
configure_file(${PROJECT_SOURCE_DIR}/include/${PRODUCT_NAME}/client/config.h.in
    ${PROJECT_BINARY_DIR}/include/${PRODUCT_NAME}/client/config.h
)

###############################################################################
# Compile options
###############################################################################

# Warnings
if(MSVC OR MSVC_IDE)
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            /W4
            /wd4700
            /wd4996
            /wd4820
            /wd4255
            /wd4668
            -DSERIALIZATION_LOGS
            -DMESSAGE_LOGS
        )
else()
    target_compile_options(${PROJECT_NAME}
        PRIVATE
            -pedantic
            -Wconversion
            -fstrict-aliasing
            -Wall
            -Wextra
            -Wcast-align
            -Wshadow
            -DSERIALIZATION_LOGS
            -DMESSAGE_LOGS
        )
endif()

###############################################################################
# Testing
###############################################################################
if(EPROSIMA_BUILD_TESTS)
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
include(${PROJECT_SOURCE_DIR}/cmake/dev/set_sources.cmake)

# Install public headers
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
