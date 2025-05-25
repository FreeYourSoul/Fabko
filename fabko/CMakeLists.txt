cmake_minimum_required(VERSION 3.22)

#
# Core library -- core
#
add_library(core STATIC)
target_sources(core
        PUBLIC
        core/common/logging.hh
        core/common/exception.hh
        core/common/ranges_to.hh
        core/common/visitor_utils.hh
        core/common/string_utils.hh
        # SAT solving logic (not sure its ever going to be useful)
        core/logic/formula.hh
        core/logic/sat/clause.hh
        core/logic/sat/dimacs_compiler.hh
        core/logic/sat/solver.hh
        PRIVATE
        core/common/logging.cpp
        core/logic/formula.cpp
        core/logic/sat/solver.cpp
        core/logic/sat/dimacs_compiler.cpp
        core/logic/sat/helpers.hh
        core/logic/sat/helpers.cpp
)
target_include_directories(core
        PRIVATE
        ${RocksDB_INCLUDE_DIR}
        PUBLIC
        core
)
target_compile_definitions(core PUBLIC cxx_std_26)
target_link_libraries(core
        PUBLIC
        RocksDB::rocksdb fmt::fmt
        PRIVATE
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)
add_library(fabko::core ALIAS core)

#
# Agent library
#
add_library(agent OBJECT)
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
target_link_libraries(agent PRIVATE fabko::core)
add_library(fabko::agent ALIAS agent)


#
# peerboard library
#
add_library(peerboard OBJECT)
target_sources(peerboard
        PUBLIC
        peerboard/peerboard.hh
        PRIVATE
        peerboard/peerboard.cpp
        datastore/kv_rocksdb.cpp
        datastore/key_value_db.hh
        datastore/kv_rocksdb.hh)
target_include_directories(peerboard PUBLIC
        peerboard
        datastore
        core
)
target_link_libraries(peerboard PRIVATE fabko::agent fabko::core)
target_compile_definitions(peerboard PUBLIC cxx_std_26)
add_library(fabko::peerboard ALIAS peerboard)


#
# Fabko
#
add_executable(fabko)

target_sources(fabko
        PRIVATE
        fabko.cpp
)
target_include_directories(fabko PUBLIC include)
target_link_libraries(fabko PRIVATE fabko::peerboard)
target_compile_definitions(fabko PUBLIC cxx_std_26)
