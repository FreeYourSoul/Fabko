cmake_minimum_required(VERSION 3.22)

#
# Core library -- compiler
#
add_library(common STATIC)
target_sources(common
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/common/logging.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/common/exception.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/common/ranges_to.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/common/visitor_utils.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/common/string_utils.hh
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common/logging.cpp
)
target_include_directories(common
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(common
        PUBLIC
        fys::fil
        spdlog::spdlog
)

add_library(fabko::common ALIAS common)

#
# fabko modules
add_subdirectory(compiler)
add_subdirectory(agent)

#
# Fabko
#
add_executable(fabko
        cli_base.hh)

target_sources(fabko
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fabko.cpp
)
target_include_directories(fabko PUBLIC include)
target_link_libraries(fabkof
        PRIVATE
        fabko::agent
        fabko::compiler)
target_compile_features(agent PUBLIC cxx_std_26)
