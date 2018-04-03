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

cmake_minimum_required(VERSION 2.8.12)
if (${CMAKE_VERSION} VERSION_GREATER 3.0)
    cmake_policy(SET CMP0048 OLD)
endif()

project(ShapeDemoClient C)

file(GLOB TEST_SOURCES main.c)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} micrortps-client)

file(COPY ${PROJECT_SOURCE_DIR}/../../test/shape_topic.xml
DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/../../test/data_reader_profile.xml
DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/../../test/data_writer_profile.xml
DESTINATION ${PROJECT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME}
RUNTIME DESTINATION examples/${PRODUCT_NAME}/${PROJECT_NAME}/${BIN_INSTALL_DIR}
)

install(FILES ${PROJECT_SOURCE_DIR}/../../test/shape_topic.xml
                ${PROJECT_SOURCE_DIR}/../../test/data_reader_profile.xml
                ${PROJECT_SOURCE_DIR}/../../test/data_writer_profile.xml
        DESTINATION examples/${PRODUCT_NAME}/${PROJECT_NAME}/${BIN_INSTALL_DIR}
)
