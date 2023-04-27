cmake_minimum_required(VERSION 3.23)

#
# Core library -- blackboard
#
add_library(core
        blackboard/blackboard.cpp
        blackboard/blackboard.hh
        blackboard/boardcom.cpp
        blackboard/boardcom.hh
        )
target_include_directories(core PUBLIC
        common
        blackboard
        )
target_compile_definitions(core PUBLIC cxx_std_23)
add_library(fabko::core ALIAS core)

#
# Agent library
#
add_library(agent
        agent/agent.cpp
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
add_library(peerboard
        peerboard/peerboard.cpp
        peerboard/peerboard.hh
        )
target_include_directories(peerboard PUBLIC
        src
        src/peerboard
        src/datastore
        datastore/kv_rocksdb.cpp
        datastore/key_value_db.hh
        datastore/kv_rocksdb.hh
        )
target_link_libraries(peerboard PRIVATE fabko::agent fabko::core)
target_compile_definitions(peerboard PUBLIC cxx_std_23)
add_library(fabko::peerboard ALIAS peerboard)


#
# Fabko
#
add_executable(fabko
        fabko.cpp
        )
target_include_directories(fabko PUBLIC
        src
        )
target_link_libraries(fabko PRIVATE fabko::peerboard)
target_compile_definitions(fabko PUBLIC cxx_std_23)
