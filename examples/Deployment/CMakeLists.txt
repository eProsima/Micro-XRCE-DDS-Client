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
    cmake_policy(SET CMP0048 NEW)
endif()


if(NOT PROFILE_CREATE_ENTITIES_XML OR
   NOT PROFILE_READ_ACCESS OR
   NOT PROFILE_WRITE_ACCESS OR
   NOT PROFILE_UDP_TRANSPORT)
    message(WARNING "Can not compile example: The profiles, PROFILE_CREATE_ENTITIES_XML, PROFILE_READ_ACCESS, PROFILE_WRITE_ACCESS and PROFILE_UDP_TRANSPORT must be enabled.")
else()

    #############################################################
    ###                     CONFIGURATOR
    #############################################################
    project(ConfiguratorClient)
    add_executable(ConfiguratorClient configurator_client.c)
    if(MSVC OR MSVC_IDE)
        target_compile_options(${PROJECT_NAME} PRIVATE /wd4996)
    endif()
    target_link_libraries(ConfiguratorClient microxrcedds_client)

    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION examples/uxr/client/${PROJECT_NAME}/${BIN_INSTALL_DIR}
        )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/
        DESTINATION  examples/uxr/client/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.h"
                       PATTERN "*.c"
                       PATTERN "*.idl"
        )

    #############################################################
    ###                      PUBLISHER
    #############################################################
    project(PublisherClient)
    add_executable(PublisherClient publisher.c HelloWorld.c)
    if(MSVC OR MSVC_IDE)
        target_compile_options(${PROJECT_NAME} PRIVATE /wd4996)
    endif()
    target_link_libraries(PublisherClient microxrcedds_client)

    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION examples/uxr/client/${PROJECT_NAME}/${BIN_INSTALL_DIR}
        )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/
        DESTINATION  examples/uxr/client/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.h"
                       PATTERN "*.c"
                       PATTERN "*.idl"
        )

    #############################################################
    ###                      SUBSCRIBER
    #############################################################
    project(SubscriberClient)
    add_executable(SubscriberClient subscriber.c HelloWorld.c)
    if(MSVC OR MSVC_IDE)
        target_compile_options(${PROJECT_NAME} PRIVATE /wd4996)
    endif()
    target_link_libraries(SubscriberClient microxrcedds_client)

    install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION examples/uxr/client/${PROJECT_NAME}/${BIN_INSTALL_DIR}
        )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/
        DESTINATION  examples/uxr/client/${PROJECT_NAME}
        FILES_MATCHING PATTERN "*.h"
                       PATTERN "*.c"
                       PATTERN "*.idl"
        )
endif()
