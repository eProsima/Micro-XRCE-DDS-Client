# Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

    # If Windows, link against export dll functions
    if(WIN32)
        add_definitions(-DMICROCDR_DYN_LINK)
    endif()

    set(CMAKE_CXX_STANDARD 11)
    set(CLIENT_UNITTESTS_SOURCE XRCEClient.cpp)
    add_executable(ClientUnitTests ${CLIENT_UNITTESTS_SOURCE})
    add_gtest(ClientUnitTests ${CLIENT_UNITTESTS_SOURCE})
    target_include_directories(ClientUnitTests PRIVATE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(ClientUnitTests microcdr micrortps-client ${GTEST_BOTH_LIBRARIES})

endif()