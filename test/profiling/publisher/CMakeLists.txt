# Copyright 2020 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

cmake_minimum_required(VERSION 3.5)

option(UCLIENT_PROFILING_XML "Enable entity creation via XML specification" OFF)
option(UCLIENT_PROFILING_REF "Enable entity creation via references" ON)
option(UCLIENT_PROFILING_BEST_EFFORT "Enable best-effort communication" OFF)
option(UCLIENT_PROFILING_RELIABLE "Enable reliable communication" ON)

project(microxrcedds-pub-profiling)

# Generate config.h
configure_file(${PROJECT_SOURCE_DIR}/../resources/config.h.in
    ${PROJECT_BINARY_DIR}/include/profiling/ref_rel/profile_config.h
    )

find_package(microxrcedds_client)

add_executable(pub_ref_rel
        main.c
        ../resources/ByteArray.c
    )

target_include_directories(pub_ref_rel
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/profiling/ref_rel>
    )

set(UCLIENT_PROFILING_REF OFF)
set(UCLIENT_PROFILING_XML ON)

configure_file(${PROJECT_SOURCE_DIR}/../resources/config.h.in
    ${PROJECT_BINARY_DIR}/include/profiling/xml_rel/profile_config.h
    )

add_executable(pub_xml_rel
        main.c
        ../resources/ByteArray.c
    )

target_include_directories(pub_xml_rel
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/profiling/xml_rel>
    )

set(UCLIENT_PROFILING_BEST_EFFORT ON)
set(UCLIENT_PROFILING_RELIABLE OFF)

configure_file(${PROJECT_SOURCE_DIR}/../resources/config.h.in
    ${PROJECT_BINARY_DIR}/include/profiling/xml_best_effort/profile_config.h
    )

add_executable(pub_xml_best_eff
        main.c
        ../resources/ByteArray.c
    )

target_include_directories(pub_xml_best_eff
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/profiling/xml_best_effort>
    )

set(UCLIENT_PROFILING_XML OFF)
set(UCLIENT_PROFILING_REF ON)

configure_file(${PROJECT_SOURCE_DIR}/../resources/config.h.in
    ${PROJECT_BINARY_DIR}/include/profiling/ref_best_effort/profile_config.h
    )

add_executable(pub_ref_best_eff
        main.c
        ../resources/ByteArray.c
    )

target_include_directories(pub_ref_best_eff
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/profiling/ref_best_effort>
    )

target_link_libraries(pub_ref_rel
        PRIVATE
            microxrcedds_client
    )

target_link_libraries(pub_xml_rel
        PRIVATE
            microxrcedds_client
    )

target_link_libraries(pub_ref_best_eff
        PRIVATE
            microxrcedds_client
    )

target_link_libraries(pub_xml_best_eff
        PRIVATE
            microxrcedds_client
    )

install(
    TARGETS
        pub_ref_rel
        pub_xml_rel
        pub_ref_best_eff
        pub_xml_best_eff
    RUNTIME DESTINATION
        bin
)
