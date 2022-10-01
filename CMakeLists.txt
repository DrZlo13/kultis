cmake_minimum_required(VERSION 3.1.0)

set(TARGET "bigbro" CACHE STRING "Framework target option")
set(TARGET_PATH "target/${TARGET}")

if(NOT "$CACHE{TARGET_CACHE}" STREQUAL "${TARGET}")
    message(STATUS "Target changed, cleaning up")
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}")
endif()

set(TARGET_CACHE ${TARGET} CACHE STRING "Framework target option cache")

include("${TARGET_PATH}/toolchain.cmake")

project("kultis" LANGUAGES C CXX ASM)

add_subdirectory("${TARGET_PATH}")

include_directories("${CMAKE_SOURCE_DIR}/app")

file(GLOB_RECURSE CORE_SOURCES "app/*.c" "app/*.cpp")

add_executable("${PROJECT_NAME}_${TARGET}" ${CORE_SOURCES})

target_link_libraries("${PROJECT_NAME}_${TARGET}" PRIVATE "kultis_target_${TARGET}")

include("${TARGET_PATH}/target.cmake")