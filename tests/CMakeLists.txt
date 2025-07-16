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
target_compile_definitions(test_datastore PUBLIC cxx_std_26)
target_link_libraries(test_datastore
        PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(test_datastore)

# Test of the Agent
#
#
add_executable(test_agent)
target_sources(test_agent
        PRIVATE
        agent/test_acl.cpp
)
target_compile_definitions(test_agent PUBLIC cxx_std_26)
target_link_libraries(test_agent
        PRIVATE Catch2::Catch2WithMain fabko::agent)

catch_discover_tests(test_agent)


# Test of the compiler
#
#add_executable(test_core)
#target_sources(test_core
#        PRIVATE
#
#)
#target_compile_definitions(test_core PUBLIC cxx_std_26)
#target_compile_definitions(test_core
#        PRIVATE -DTEST_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/_assets")
#target_link_libraries(test_core
#        PRIVATE fabko::core Catch2::Catch2WithMain)
#
#catch_discover_tests(test_core)

