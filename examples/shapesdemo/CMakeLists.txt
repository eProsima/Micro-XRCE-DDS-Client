cmake_minimum_required(VERSION 2.8.12)

project(shapes_demo_client C)

file(GLOB TEST_SOURCES main.c)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} micrortps-client pthread)

file(COPY ${PROJECT_SOURCE_DIR}/../../test/shape_topic.xml
DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/../../test/data_reader_profile.xml
DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/../../test/data_writer_profile.xml
DESTINATION ${PROJECT_BINARY_DIR})

install(TARGETS ${PROJECT_NAME}
RUNTIME DESTINATION examples/shapesdemo/${BIN_INSTALL_DIR}
)

install(FILES ${PROJECT_SOURCE_DIR}/../../test/shape_topic.xml
${PROJECT_SOURCE_DIR}/../../test/data_reader_profile.xml
${PROJECT_SOURCE_DIR}/../../test/data_writer_profile.xml
DESTINATION examples/shapesdemo/${BIN_INSTALL_DIR}
)