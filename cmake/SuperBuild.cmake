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

include(ExternalProject)

unset(_deps)

# Micro CDR.
find_package(microcdr "1.0.1" EXACT QUIET)
if(NOT microcdr_FOUND)
    ExternalProject_Add(ucdr
        DOWNLOAD_COMMAND
            git submodule update --init ${PROJECT_SOURCE_DIR}/thirdparty/microcdr/
        PREFIX
            ${PROJECT_BINARY_DIR}/ucdr
        SOURCE_DIR
            ${PROJECT_SOURCE_DIR}/thirdparty/microcdr
        INSTALL_DIR
            ${PROJECT_BINARY_DIR}/temp_install
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        )
    LIST(APPEND _deps ucdr)
endif()

# Client project.
ExternalProject_Add(uclient
    SOURCE_DIR
        ${PROJECT_SOURCE_DIR}
    BINARY_DIR
        ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_ARGS
        -DSUPERBUILD=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    INSTALL_COMMAND
        ""
    DEPENDS
        ${_deps}
    )
