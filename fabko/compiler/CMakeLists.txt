add_library(compiler STATIC
        backend/sat/cli.hh)
target_sources(compiler
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/sat/solver.hh
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/Metadata.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/sat/solver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/sat/solver_context.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/backend/sat/solver_context.hh
)
target_include_directories(compiler
        PRIVATE
        ${RocksDB_INCLUDE_DIR}
)
target_compile_definitions(compiler PUBLIC cxx_std_26)
target_link_libraries(compiler
        fabko::common
        nlohmann_json::nlohmann_json
)
add_library(fabko::compiler ALIAS compiler)
