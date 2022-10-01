cmake_minimum_required(VERSION 3.1.0)

project("kultis_target_flipper")

file(GLOB_RECURSE TARGET_SOURCES "*.c" "*.cpp")

add_library(${PROJECT_NAME} SHARED ${TARGET_SOURCES})
