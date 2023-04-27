cmake_minimum_required(VERSION 3.22)

set(project_version 0.0.1)
set(project_description "Fabko is a set of library for server / client execution of distributed multi-agent application.")

project(fabko
        VERSION ${project_version}
        DESCRIPTION ${project_description}
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

find_package(RocksDB CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_subdirectory(fabko)
add_subdirectory(tests)
