cmake_minimum_required(VERSION 3.1.0)

set(TARGET "bigbro" CACHE STRING "Framework target option")
set(TARGET_PATH "target/${TARGET}")
include("${TARGET_PATH}/toolchain.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

project("kultis" LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

add_subdirectory("${TARGET_PATH}")

include_directories("${CMAKE_SOURCE_DIR}/app")
include_directories("${CMAKE_SOURCE_DIR}/target/common/includes")

file(GLOB_RECURSE CORE_SOURCES
    "app/*.c"
    "app/*.cpp"
    "target/common/source/*.c"
    "target/common/source/*.cpp"
)

add_executable("${PROJECT_NAME}_${TARGET}" ${CORE_SOURCES})

target_link_libraries("${PROJECT_NAME}_${TARGET}" PRIVATE "kultis_target_${TARGET}")

include("${TARGET_PATH}/target.cmake")