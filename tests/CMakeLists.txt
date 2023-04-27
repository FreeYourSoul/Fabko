cmake_minimum_required(VERSION 3.22)

enable_testing()

find_package(Catch2 3 CONFIG REQUIRED)

include(CTest)
include(Catch)

# Test of the datastore
#
add_executable(test_datastore)
target_sources(test_datastore
        PRIVATE
        catch_main.cpp
        peerboard/test_kv_rocksdb.cpp
        )
target_link_libraries(test_datastore
        PRIVATE Catch2::Catch2 fabko::peerboard)

# Test of the core
#
add_executable(test_core)
target_sources(test_core
        PRIVATE
        catch_main.cpp
        core/test_assignment_bitset.cpp
        core/sat/test_basic_sat.cpp
        )
target_link_libraries(test_core
        PRIVATE Catch2::Catch2 fabko::core)

