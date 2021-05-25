cmake_minimum_required(VERSION 3.17)
project(cub_array)

set(CMAKE_CXX_STANDARD 17)

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third-party"
)

add_library(cub_array library.cpp library.h include/cub/array/ObjectArray.h include/cub/base/Placement.h)