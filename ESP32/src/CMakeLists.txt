# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(UI_DIR ../lib/ui_library)
file(GLOB_RECURSE UI_SOURCES ${UI_DIR}/*.c)

set(UTIL_DIR ../lib/util)
file(GLOB_RECURSE UTIL_SOURCES ${UTIL_DIR}/*.c)

set(CONFIG_DIR ../config)

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(SRCS ${app_sources} ${UI_SOURCES} ${UTIL_SOURCES}
    INCLUDE_DIRS "."  ${UI_DIR} ${UTIL_DIR} ${CONFIG_DIR})
