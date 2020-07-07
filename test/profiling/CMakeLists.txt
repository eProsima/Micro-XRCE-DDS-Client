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

option(UCLIENT_PROFILING_XML            "Enable entity creation via XML specification"  OFF)
option(UCLIENT_PROFILING_REF            "Enable entity creation via references"         ON )
option(UCLIENT_PROFILING_BEST_EFFORT    "Enable best-effort communication"              OFF)
option(UCLIENT_PROFILING_RELIABLE       "Enable reliable communication"                 ON )
option(UCLIENT_PROFILING_INFINITE_LOOP  "Enable infinite loop topic publication"        OFF)

set(PROFILING_SRC_DIR   ${PROJECT_SOURCE_DIR}/test/profiling)
set(RESOURCES_DIR       ${PROFILING_SRC_DIR}/resources)
set(PROFILING_BIN_DIR   ${PROJECT_BINARY_DIR}/test/profiling)

set(PUBLISHER_FOLDER_ID   publisher)
set(SUBSCRIBER_FOLDER_ID subscriber)

set(PUBLISHER_ID         pub)
set(SUBSCRIBER_ID        sub)
set(BEST_EFFORT_ID  best_eff)
set(RELIABLE_ID          rel)
set(CREATION_XML_ID      xml)
set(CREATION_REF_ID      ref)

project(microxrcedds-client-profiling)

###############################################################################
# @brief Helper macro to create a publisher/subscriber application.
# @param ENTITY         Publisher / Subscriber.
# @param SOURCES_DIR    Directory name containing sources for main application.
# @param CREATION_MODE  DDS entities creation mode (XML / reference file).
# @param STREAM_MODE    XRCE-DDS stream communication mode.
###############################################################################
macro(uclient_create_profiling_app ENTITY SOURCES_DIR CREATION_MODE STREAM_MODE)
    set(TARGET_NAME ${ENTITY}_${CREATION_MODE}_${STREAM_MODE})
    configure_file(${RESOURCES_DIR}/config.h.in
            ${PROJECT_BINARY_DIR}/include/profiling/${CREATION_MODE}_${STREAM_MODE}/profile_config.h
        )

    add_executable(${TARGET_NAME}
            ${SOURCES_DIR}/main.c
            ${RESOURCES_DIR}/ByteArray.c
        )

    if(MSVC OR MSVC_IDE)
        target_compile_options(${TARGET_NAME}
            PRIVATE
                /wd4996
            )
    endif()

    set_target_properties(${TARGET_NAME}
        PROPERTIES
            C_STANDARD 99
            C_STANDARD_REQUIRED YES
            RUNTIME_OUTPUT_DIRECTORY ${PROFILING_BIN_DIR}/${SOURCES_DIR}
        )

    target_include_directories(${TARGET_NAME}
        PRIVATE
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/profiling/${CREATION_MODE}_${STREAM_MODE}>
            ${RESOURCES_DIR}
        )

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            microxrcedds_client
        )
endmacro()

############################################################################################
# Create publisher and subscriber applications, using references and reliable communication.
############################################################################################
uclient_create_profiling_app(${PUBLISHER_ID}  ${PUBLISHER_FOLDER_ID}  ${CREATION_REF_ID} ${RELIABLE_ID})
uclient_create_profiling_app(${SUBSCRIBER_ID} ${SUBSCRIBER_FOLDER_ID} ${CREATION_REF_ID} ${RELIABLE_ID})

#################################################################################################
# Create publisher and subscriber applications, using xml definitions and reliable communication.
#################################################################################################
set(UCLIENT_PROFILING_REF OFF)
set(UCLIENT_PROFILING_XML ON)

uclient_create_profiling_app(${PUBLISHER_ID}  ${PUBLISHER_FOLDER_ID}  ${CREATION_XML_ID} ${RELIABLE_ID})
uclient_create_profiling_app(${SUBSCRIBER_ID} ${SUBSCRIBER_FOLDER_ID} ${CREATION_XML_ID} ${RELIABLE_ID})

###############################################################################################
# Create infinite loop publisher application, using xml definitions and reliable communication.
###############################################################################################
set(UCLIENT_PROFILING_INFINITE_LOOP ON)
uclient_create_profiling_app(${PUBLISHER_ID}  ${PUBLISHER_FOLDER_ID}  ${CREATION_XML_ID} ${RELIABLE_ID}_loop)
set(UCLIENT_PROFILING_INFINITE_LOOP OFF)

####################################################################################################
# Create publisher and subscriber applications, using xml definitions and best-effort communication.
####################################################################################################
set(UCLIENT_PROFILING_BEST_EFFORT ON)
set(UCLIENT_PROFILING_RELIABLE OFF)

uclient_create_profiling_app(${PUBLISHER_ID}  ${PUBLISHER_FOLDER_ID}  ${CREATION_XML_ID} ${BEST_EFFORT_ID})
uclient_create_profiling_app(${SUBSCRIBER_ID} ${SUBSCRIBER_FOLDER_ID} ${CREATION_XML_ID} ${BEST_EFFORT_ID})

###############################################################################################
# Create publisher and subscriber applications, using references and best-effort communication.
###############################################################################################
set(UCLIENT_PROFILING_XML OFF)
set(UCLIENT_PROFILING_REF ON)

uclient_create_profiling_app(${PUBLISHER_ID}  ${PUBLISHER_FOLDER_ID}  ${CREATION_REF_ID} ${BEST_EFFORT_ID})
uclient_create_profiling_app(${SUBSCRIBER_ID} ${SUBSCRIBER_FOLDER_ID} ${CREATION_REF_ID} ${BEST_EFFORT_ID})

###############################################################################
# Install targets
###############################################################################
install(
    TARGETS
        ${PUBLISHER_ID}_${CREATION_XML_ID}_${RELIABLE_ID}
        ${PUBLISHER_ID}_${CREATION_REF_ID}_${RELIABLE_ID}
        ${PUBLISHER_ID}_${CREATION_XML_ID}_${BEST_EFFORT_ID}
        ${PUBLISHER_ID}_${CREATION_REF_ID}_${BEST_EFFORT_ID}
        ${SUBSCRIBER_ID}_${CREATION_XML_ID}_${RELIABLE_ID}
        ${SUBSCRIBER_ID}_${CREATION_REF_ID}_${RELIABLE_ID}
        ${SUBSCRIBER_ID}_${CREATION_XML_ID}_${BEST_EFFORT_ID}
        ${SUBSCRIBER_ID}_${CREATION_REF_ID}_${BEST_EFFORT_ID}
        ${PUBLISHER_ID}_${CREATION_XML_ID}_${RELIABLE_ID}_loop
    RUNTIME DESTINATION
        bin
    )

