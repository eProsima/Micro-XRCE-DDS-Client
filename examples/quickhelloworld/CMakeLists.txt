cmake_minimum_required(VERSION 2.8.12)

project(quick_hello_world_client)

file(GLOB TEST_SOURCES main.c)
add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} micrortps-client pthread)

file(COPY ${PROJECT_SOURCE_DIR}/../../test/hello_topic.xml
     DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/../../test/hello_data_reader_profile.xml
     DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/../../test/hello_data_writer_profile.xml
     DESTINATION ${PROJECT_BINARY_DIR})