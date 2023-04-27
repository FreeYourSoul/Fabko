cmake_minimum_required(VERSION 3.22)
project(fabko CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(RocksDB REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(src)
