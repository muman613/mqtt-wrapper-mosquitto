cmake_minimum_required(VERSION 3.30.0)

project(mqtt-service)

option(ENABLE_DEBUG_LOGGING "Enable verbose Qt and Mosquitto debug logging" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    message(STATUS "Build type not specified: Use Release by default")
endif(NOT CMAKE_BUILD_TYPE)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

add_subdirectory(sample/mqttwrapper)
