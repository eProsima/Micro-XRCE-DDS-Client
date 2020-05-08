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

cmake_minimum_required(VERSION 3.5.0 FATAL_ERROR)

project(RequestAdder)

if(NOT PROFILE_UDP_TRANSPORT)
    message(WARNING "Can not compile example: the PROFILE_UDP_TRANSPORT must be enables.")
else()
    add_executable(${PROJECT_NAME} main.c)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_compile_options(${PROJECT_NAME} PRIVATE /wd4996)
    endif()

    set_target_properties(${PROJECT_NAME} PROPERTIES
        C_STANDARD
            99
        C_STANDARD_REQUIRED
            YES
        )

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            microxrcedds_client
        )

    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION
            examples/uxr/client/${PROJECT_NAME}/${BIN_INSTALL_DIR}
        )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}
        DESTINATION
            examples/uxr/client/${PROJECT_NAME}
        FILES_MATCHING
            PATTERN "*.h"
            PATTERN "*.c"
            PATTERN "*.idl"
        )
endif()