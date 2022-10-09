cmake_minimum_required(VERSION 3.1.0)

project("kultis_target_flipper")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../common/includes")

file(GLOB_RECURSE TARGET_SOURCES "*.c" "*.cpp")

add_library(${PROJECT_NAME} SHARED ${TARGET_SOURCES})
