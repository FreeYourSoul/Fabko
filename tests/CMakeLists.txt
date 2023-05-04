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
        peerboard/test_kv_rocksdb.cpp
        )
target_link_libraries(test_datastore
        PRIVATE Catch2::Catch2WithMain fabko::peerboard)

catch_discover_tests(test_datastore)


# Test of the core
#
add_executable(test_core)
target_sources(test_core
        PRIVATE
        core/common/test_assignment_bitset.cpp
        core/common/test_algo_utils.cpp
        core/logic/test_formula.cpp
        core/sat/test_dimacs_compiler.cpp
        core/sat/test_basic_sat.cpp
        core/sat/test_sat_coloring.cpp
        )
target_compile_definitions(test_core
        PRIVATE -DTEST_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/_assets")
target_link_libraries(test_core
        PRIVATE fabko::core Catch2::Catch2WithMain)

catch_discover_tests(test_core)

