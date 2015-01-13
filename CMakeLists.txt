cmake_minimum_required(VERSION 2.8.4)
project(csgo_external)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -std=c++11")

set(SOURCE_FILES log.cpp remote.cpp netvar.cpp hack.cpp main.cpp)
add_executable(csgo_external ${SOURCE_FILES})