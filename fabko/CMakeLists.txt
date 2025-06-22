cmake_minimum_required(VERSION 3.22)

#
# Core library -- compiler
#
add_library(common STATIC)
target_sources(common
        PUBLIC
        common/logging.hh
        common/exception.hh
        common/ranges_to.hh
        common/visitor_utils.hh
        common/string_utils.hh
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common/logging.cpp
)
target_include_directories(common
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(common
        PUBLIC
        spdlog::spdlog
        fmt::fmt)

add_library(compiler STATIC)
target_sources(compiler
        PUBLIC
        compiler/backend/sat/solver.hh
        PRIVATE
        compiler/backend/sat/solver.cpp
        compiler/backend/sat/solver_context.cpp
        compiler/backend/sat/solver_context.hh
)
target_include_directories(compiler
        PRIVATE
        ${RocksDB_INCLUDE_DIR}
)
target_compile_definitions(compiler PUBLIC cxx_std_26)
target_link_libraries(compiler
        PUBLIC
        RocksDB::rocksdb fmt::fmt
        PRIVATE
        common
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)
add_library(fabko::compiler ALIAS compiler)

#
# Agent library
#
add_library(agent OBJECT
        common/soa/soa.h)
target_sources(agent
        PUBLIC
        agent/agent.hh
        agent/action.hh
        agent/protocol/acl.hh
        agent/protocol/fap_request.hh
        agent/protocol/agent_com.hh
        PRIVATE
        agent/agent.cpp
        agent/blackboard.cpp
        agent/blackboard.hh
        agent/protocol/agent_com_local.cpp
        agent/protocol/agent_com_p2p.cpp
)
target_include_directories(agent PUBLIC agent)
target_compile_definitions(agent PUBLIC cxx_std_26)
target_link_libraries(agent PRIVATE common)
add_library(fabko::agent ALIAS agent)

#
# Fabko
#
add_executable(fabko)

target_sources(fabko
        PRIVATE
        fabko.cpp
)
target_include_directories(fabko PUBLIC include)
target_link_libraries(fabko
        PRIVATE
        fabko::agent
        fabko::compiler)
target_compile_definitions(fabko PUBLIC cxx_std_26)
