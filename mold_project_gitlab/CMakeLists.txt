cmake_minimum_required(VERSION 3.14)
project(wolf LANGUAGES C CXX)

message(STATUS "CMAKE_PREFIX_PATH:${CMAKE_PREFIX_PATH}")

set(CMAKE_CXX_STANDARD 17)

# warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -Wno-unused-local-typedefs -Wno-unused-parameter -Wno-unused-variable -Wno-ignored-qualifiers -Wno-psabi")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -lrt")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -rdynamic")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rdynamic")

# set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(Qt5 REQUIRED Quick Gui Widgets LinguistTools DBus Qml QuickWidgets)
find_package(Boost 1.69 REQUIRED COMPONENTS
  filesystem
  iostreams
  log
  log_setup
  program_options
  serialization
  system
  thread
  timer
  unit_test_framework
  date_time
)
find_package(Enocean REQUIRED)
find_package(Soci REQUIRED)
find_package(WebsocketPP REQUIRED)
# find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(WiringPi REQUIRED)
find_package(WebsocketPP REQUIRED)
# CONFIG because: use the .cmake from the google_test/lib and not the one that comes with cmake
find_package(GTest REQUIRED CONFIG)

add_definitions(-DBOOST_ALL_DYN_LINK)

if(NOT BOND_DIR)
    message(FATAL_ERROR "BOND_DIR not set")
endif(NOT BOND_DIR)

set(BOND_GBC ${BOND_DIR}/bond-install/bin/gbc)

set(WOLF_ALL_INCLUDES
    ${BOND_DIR}/bond-install/include/
    ${BOND_DIR}/bond/thirdparty/rapidjson/include
    ${SOCI_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
)

# VERSION
include(${CMAKE_HOME_DIRECTORY}/cmake/version.cmake)

mold_determine_versions()
mold_init_versions()
mold_set_version_strings()

message(STATUS "building version: ${MOLD_VERSION}+${GIT_COMMIT_HASH}")


option(DOCUMENTATION OFF)
option(WITH_CJSON OFF)
option(WITH_CLIENTS OFF)
option(WITH_BROKER OFF)
option(WITH_APPS OFF)
option(WITH_PLUGINS OFF)
option(WITH_DLT OFF)
option(WITH_WEBSOCKETS OFF)
option(WITH_SOCKS OFF)
option(WITH_TLS OFF)
option(WITH_PIC OFF)

add_subdirectory(mqtt)
add_subdirectory(boost/asio)
add_subdirectory(boost/stacktrace)
add_subdirectory(boost/di)
add_subdirectory(wolf)
add_subdirectory(mold)
add_subdirectory(wifi)
add_subdirectory(tests_gtest)
add_subdirectory(enocean_test)
add_subdirectory(tone_test)
add_subdirectory(hardware_information)
add_subdirectory(sensors)
add_subdirectory(gsm_confirmation)
add_subdirectory(gsm_mux)
add_subdirectory(log)
add_subdirectory(keyboard_test)
add_subdirectory(mold_values_test)
