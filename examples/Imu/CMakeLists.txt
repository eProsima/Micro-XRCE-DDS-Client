
cmake_minimum_required(VERSION 2.8.12)
if (${CMAKE_VERSION} VERSION_GREATER 3.0)
    cmake_policy(SET CMP0048 NEW)
endif()

project(client_examples)

#find_package(microcdr REQUIRED)
#find_package(microxrcedds_client REQUIRED)

set(SRCS
    Imu.c
    )

# Imu Subscriber
add_executable(ImuSubscriber ImuSubscriber.c ${SRCS})
target_link_libraries(ImuSubscriber microxrcedds_client)
set_target_properties(ImuSubscriber PROPERTIES
    C_STANDARD
        99
    C_STANDARD_REQUIRED
        YES
    )
target_compile_options(ImuSubscriber
    PRIVATE
        $<$<C_COMPILER_ID:MSVC>:/W4 /wd4700 /wd4996 /wd4820 /wd4255 /wd4668>
        $<$<C_COMPILER_ID:GNU>:-pedantic -fstrict-aliasing -Wall -Wcast-align -Wshadow>
    )

# Imu Publisher
add_executable(ImuPublisher ImuPublisher.c ${SRCS})
target_link_libraries(ImuPublisher microxrcedds_client)
set_target_properties(ImuPublisher PROPERTIES
    C_STANDARD
        99
    C_STANDARD_REQUIRED
        YES
    )
target_compile_options(ImuPublisher
    PRIVATE
        $<$<C_COMPILER_ID:MSVC>:/W4 /wd4700 /wd4996 /wd4820 /wd4255 /wd4668>
        $<$<C_COMPILER_ID:GNU>:-pedantic -fstrict-aliasing -Wall -Wcast-align -Wshadow>
    )

