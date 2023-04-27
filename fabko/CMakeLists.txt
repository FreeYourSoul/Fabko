cmake_minimum_required(VERSION 3.22)

#
# Core library -- core
#
add_library(core STATIC)
target_sources(core
        PUBLIC
        core/blackboard.cpp
        core/blackboard.hh
        core/boardcom.cpp
        core/boardcom.hh
        core/common/exception.hh
        core/common/utils.hh
        core/sat/dimacs_compiler.cpp
        core/sat/dimacs_compiler.hh
        core/sat/solver.cpp
        core/sat/solver.hh
        )
target_include_directories(core
        PRIVATE
        ${RocksDB_INCLUDE_DIR}
        datastore/include/
        PUBLIC
        core/common
        core
        )
target_compile_definitions(core PUBLIC cxx_std_23)
target_link_libraries(core
        PUBLIC
        RocksDB::rocksdb fmt::fmt
        PRIVATE
        nlohmann_json::nlohmann_json)
add_library(fabko::core ALIAS core)

#
# Agent library
#
add_library(agent OBJECT)
target_sources(agent
        PRIVATE
        agent/agent.cpp
        PUBLIC
        agent/agent.hh)
target_include_directories(agent PUBLIC
        agent
        )
target_link_libraries(agent PRIVATE fabko::core)
target_compile_definitions(agent PUBLIC cxx_std_23)
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
        )
target_link_libraries(peerboard PRIVATE fabko::agent fabko::core)
target_compile_definitions(peerboard PUBLIC cxx_std_23)
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
target_compile_definitions(fabko PUBLIC cxx_std_23)
