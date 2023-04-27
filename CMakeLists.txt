cmake_minimum_required(VERSION 3.23)
project(fabko CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(RocksDB REQUIRED)

add_subdirectory(src)
