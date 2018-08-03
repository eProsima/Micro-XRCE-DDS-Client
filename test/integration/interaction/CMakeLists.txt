###############################################################################
#
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
#
###############################################################################

project(interaction-intest-client CXX)

set(SRCS
    ClientInteraction.cpp
    Gateway.cpp
    HelloWorld.c
    HelloWorldWriter.c
    )

add_library(${PROJECT_NAME} STATIC ${SRCS})
set_common_compile_options(${PROJECT_NAME})
if(MSVC OR MSVC_IDE)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4996)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${GTEST_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} micrortps_client ${GTEST_BOTH_LIBRARIES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    )

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )

