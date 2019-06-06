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
unset(microcdr_DIR CACHE)
find_package(microcdr "1.1.0" EXACT QUIET)
if(NOT microcdr_FOUND)
    ExternalProject_Add(ucdr
        DOWNLOAD_COMMAND
            cd ${PROJECT_SOURCE_DIR} && git submodule update --init thirdparty/microcdr/
        PREFIX
            ${PROJECT_BINARY_DIR}/ucdr
        SOURCE_DIR
            ${PROJECT_SOURCE_DIR}/thirdparty/microcdr
        INSTALL_DIR
            ${PROJECT_BINARY_DIR}/temp_install
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_SYSROOT:PATH=${CMAKE_SYSROOT}
            -DCHECK_ENDIANNESS:BOOL=${CHECK_ENDIANNESS}
        )
    list(APPEND _deps ucdr)
endif()

# Client project.
ExternalProject_Add(uclient
    SOURCE_DIR
        ${PROJECT_SOURCE_DIR}
    BINARY_DIR
        ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_CACHE_ARGS
        -DUCLIENT_SUPERBUILD:BOOL=OFF
    INSTALL_COMMAND
        ""
    DEPENDS
        ${_deps}
    )
