cmake_minimum_required(VERSION 3.23)
project(fabko)

set(CMAKE_CXX_STANDARD 23)

add_library(fabko src/fabko.cpp
        src/blackboard/blackboard.cpp
        src/blackboard/blackboard.hh
        src/peerboard/peerboard.cpp
        src/peerboard/peerboard.hh
        )
