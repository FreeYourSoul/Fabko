add_library(parser STATIC)
target_sources(parser
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/token.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/ast.hh
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer.cpp
)
target_include_directories(parser
        PRIVATE
        ${RocksDB_INCLUDE_DIR}
)
target_compile_definitions(parser PUBLIC cxx_std_23)
target_link_libraries(parser
        fabko::common
        nlohmann_json::nlohmann_json
)
add_library(fabko::parser ALIAS parser)
