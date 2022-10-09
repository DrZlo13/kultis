cmake_minimum_required(VERSION 3.1.0)

project("kultis_target_bigbro")

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qt@5/5.15.6/")

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../common/includes")

file(GLOB_RECURSE TARGET_SOURCES "*.c" "*.cpp")

add_library(${PROJECT_NAME} SHARED ${TARGET_SOURCES})

# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
