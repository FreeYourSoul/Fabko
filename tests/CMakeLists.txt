cmake_minimum_required(VERSION 3.22)

enable_testing()

find_package(Catch2 3 CONFIG REQUIRED)

add_executable(test_rocksdb)
target_sources(test_rocksdb
        PRIVATE
        test_kv_rocksdb.cpp
        )
target_link_libraries(test_rocksdb
        PRIVATE Catch2::Catch2 fabko::peerboard)
