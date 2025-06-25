add_library(agent OBJECT)
target_sources(agent
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/agent.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/action.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/protocol/acl.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/protocol/fap_request.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/protocol/agent_com.hh
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/agent.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/blackboard.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/blackboard.hh
        ${CMAKE_CURRENT_SOURCE_DIR}/protocol/agent_com_local.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/protocol/agent_com_p2p.cpp
)
target_include_directories(agent PUBLIC agent)
target_compile_definitions(agent PUBLIC cxx_std_26)

target_link_libraries(agent
        PRIVATE
        fabko::common)

add_library(fabko::agent ALIAS agent)