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

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(fmt CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fil CONFIG REQUIRED)

if (UNIX AND NOT APPLE)
    add_library(uring::uring INTERFACE IMPORTED
            tests/compiler/soa/watcher_testcase.cpp)
    set_target_properties(uring::uring PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_INCLUDE_PATH}"
            INTERFACE_LINK_LIBRARIES "uring"
    )
endif ()
find_package(RocksDB CONFIG REQUIRED)

set(nlohmann-json_IMPLICIT_CONVERSIONS OFF)

add_subdirectory(fabko)

if (IS_BUILT_FROM_SOURCE AND BUILD_TESTING)
    message(STATUS "Tests will be built")
    add_subdirectory(tests)
endif ()

#
# Installation
#
set(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Installation directory for libraries")
set(CMAKE_INSTALL_INCLUDEDIR include CACHE PATH "Installation directory for header files")

write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/fabkoConfigVersion.cmake"
        VERSION "${FABKO_VERSION}"
        COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/fabkoConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/fabkoConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/fabko"
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR
)

message("CMAKE_INSTALL_LIBDIR ----" ${CMAKE_INSTALL_LIBDIR})
message("CMAKE_INSTALL_BINDIR ----" ${CMAKE_INSTALL_BINDIR})
message("CMAKE_INSTALL_INCLUDEDIR ----" ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS fabko
        EXPORT fabkoTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(EXPORT fabkoTargets
        FILE fabkoTargets.cmake
        NAMESPACE fabko::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/fabko"
)

install(FILES
        "${PROJECT_BINARY_DIR}/fabkoConfig.cmake"
        "${PROJECT_BINARY_DIR}/fabkoConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/fabko"
)