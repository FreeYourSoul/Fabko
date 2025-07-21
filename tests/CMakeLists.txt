cmake_minimum_required(VERSION 3.22)

enable_testing()

find_package(Catch2 3 CONFIG REQUIRED)

include(CTest)
include(Catch)

#
# Test of the Agent
#
#add_executable(test_agent)
#target_sources(test_agent
#        PRIVATE
#        ${CMAKE_CURRENT_SOURCE_DIR}/agent/test_acl.cpp
#)
#target_compile_features(test_agent PUBLIC cxx_std_26)
#target_link_libraries(test_agent
#        PRIVATE Catch2::Catch2WithMain fabko::agent)
#
#catch_discover_tests(test_agent)

#
# Test of the compiler
#
add_executable(test_compiler)
target_sources(test_compiler
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler/soa/watcher_testcase.cpp
)
target_compile_features(test_compiler PUBLIC cxx_std_26)
target_link_libraries(test_compiler
        PRIVATE fabko::compiler Catch2::Catch2WithMain)

catch_discover_tests(test_compiler)

