cmake_minimum_required(VERSION 2.8.12)

project(message_example C)

file(GLOB TEST_SOURCES main.c)

add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} micrortps-client)

install(TARGETS ${PROJECT_NAME}
RUNTIME DESTINATION examples/message/${BIN_INSTALL_DIR}
)