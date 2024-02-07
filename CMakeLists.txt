cmake_minimum_required(VERSION 3.24)
project(simple_jpeg_codec)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(3rdparty)

add_executable(simple_jpeg_codec main.cpp)
