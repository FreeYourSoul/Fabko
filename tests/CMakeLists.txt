cmake_minimum_required(VERSION 3.22)

enable_testing()

find_package(Catch2 3 REQUIRED)

include(CTest)
include(Catch)

# Test of the datastore
#
add_executable(test_datastore)
target_sources(test_datastore
        PRIVATE
        catch_main.cpp
        test_kv_rocksdb.cpp
        )
target_link_libraries(test_datastore
        PRIVATE Catch2::Catch2 fabko::peerboard)

catch_discover_tests(test_datastore)

# Test of the core
#
add_executable(test_core)
target_sources(test_core
        PRIVATE
        catch_main.cpp
        core/test_assignment_bitset.cpp
        )
target_link_libraries(test_core
        PRIVATE Catch2::Catch2 fabko::core)

#catch_discover_tests(test_core)