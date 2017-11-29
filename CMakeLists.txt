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

project(micrortps-client)

###############################################################################
# eProsima build options
###############################################################################
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
find_eprosima_package(micrortps_transport)
find_eprosima_package(micro-CDR)

file(GLOB CLIENT_LIB_SOURCES
    src/c/*.c
    src/c/log/*.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSERIALIZATION_LOGS")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMESSAGE_LOGS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSERIALIZATION_LOGS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMESSAGE_LOGS")

if(NOT TARGET transport)
     add_subdirectory(thirdparty/micrortps_transport)
endif()
add_subdirectory(thirdparty/micro-CDR)

include_directories(${PROJECT_SOURCE_DIR}/thirdparty/micro-CDR/include)
include_directories(${PROJECT_SOURCE_DIR}/include/micrortps/client)
include_directories(${PROJECT_SOURCE_DIR}/src/c/logs)
include_directories(${PROJECT_SOURCE_DIR}/src/c)
add_library(${PROJECT_NAME} ${CLIENT_LIB_SOURCES})
target_link_libraries(${PROJECT_NAME} microcdr transport)
target_include_directories(
    ${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
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
    add_subdirectory(examples/helloworld)
    add_subdirectory(examples/quickhelloworld)
    add_subdirectory(examples/message)
    add_subdirectory(examples/shapesdemo)
endif()


###############################################################################
# Packaging
###############################################################################

# Install examples
install(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/
DESTINATION examples
COMPONENT examples
PATTERN "examples/*/CMakeLists.txt" EXCLUDE
)


# Install public headers
set_public_headers_directory(${PROJECT_SOURCE_DIR}/include micrortps
DESTINATION ${INCLUDE_INSTALL_DIR}
COMPONENT headers
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    COMPONENT libraries
    )