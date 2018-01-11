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

if(NOT ((MSVC OR MSVC_IDE) AND EPROSIMA_INSTALLER))
    include(${PROJECT_SOURCE_DIR}/cmake/dev/gtest.cmake)

    set(CMAKE_CXX_STANDARD 11)
    set(CLIENT_UNITTESTS_SOURCE XRCEClient.cpp)
    add_executable(ClientUnitTests ${CLIENT_UNITTESTS_SOURCE})
    add_gtest(ClientUnitTests ${CLIENT_UNITTESTS_SOURCE})
    
    # Directories
    set(MICROCDR_SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/micro-CDR)
    get_directory_property(MICROCRD_BINARY_DIR 
        DIRECTORY ${MICROCDR_SOURCE_DIR} 
	DEFINITION PROJECT_BINARY_DIR
    )

    target_include_directories(ClientUnitTests PRIVATE ${GTEST_INCLUDE_DIRS} 
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src/c
        ${PROJECT_BINARY_DIR}/include
        ${MICROCDR_SOURCE_DIR}/include
        ${MICROCDR_BINARY_DIR}/include
    )

    target_link_libraries(ClientUnitTests ${GTEST_BOTH_LIBRARIES} micrortps-client)

    # Copy .xml files.
    file(COPY ${PROJECT_SOURCE_DIR}/test DESTINATION ${PROJECT_BINARY_DIR}
         FILES_MATCHING PATTERN "*.xml")

    install(TARGETS ClientUnitTests
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION ${BIN_INSTALL_DIR}
        LIBRARY DESTINATION ${LIB_INSTALL_DIR}
        ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
        COMPONENT libraries
    )

    if(WIN32)
        string(REPLACE ";" "\\;" WIN_PATH "$ENV{PATH}")
        set_property(TEST ClientUnitTests APPEND PROPERTY ENVIRONMENT
            "PATH=$<SHELL_PATH:$<TARGET_FILE_DIR:microcdr>\\;$<TARGET_FILE_DIR:transport>\\;${WIN_PATH}>"
	)
    endif()
        

export(TARGETS ClientUnitTests FILE ${PROJECT_BINARY_DIR}/cmake/config/${PROJECT_NAME}Targets.cmake)

endif()
