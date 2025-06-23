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
find_package(fil CONFIG REQUIRED)

if (UNIX AND NOT APPLE)
    add_library(uring::uring INTERFACE IMPORTED)
    set_target_properties(uring::uring PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_INCLUDE_PATH}"
            INTERFACE_LINK_LIBRARIES "uring"
    )
endif ()
find_package(RocksDB CONFIG REQUIRED)

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

add_subdirectory(fabko)
add_subdirectory(tests)
