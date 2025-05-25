cmake_minimum_required(VERSION 3.22)

set(project_description "Fabko is a set of library for server / client execution of distributed multi-agent application.")

include(cmake/utils.cmake)

fabko_extract_version()

message("Version Fabko :: ${FABKO_VERSION}\n")

project(fabko
        VERSION ${FABKO_VERSION}
        DESCRIPTION ${project_description}
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

add_subdirectory(fabko)
add_subdirectory(tests)
