// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//
#include <functional>

#include "blackboard.hh"
#include "common/visitor_utils.hh"

namespace fabko {

struct blackboard::blackboard_impl {

    blackboard_data instantiate_black_board(const std::string& request) {
        std::visit(
            overloaded{[&request](auto& b) -> std::string { return b.instantiate_blackboard(request); }}, bc);
        return data;
    }

    agent_protocol::agent_com bc;
    blackboard_data data;
};

blackboard::~blackboard() = default;

template<agent_protocol::c_communicate BoardCommunication>
blackboard::blackboard(BoardCommunication&& bc, agent_protocol::request initial_request)
    : _pimpl{std::forward<BoardCommunication>(bc), {.initial_request = std::move(initial_request)}} {
}

agent_protocol::propositions blackboard::request_propositions(const acl::message& request) {
    return {};
}

agent_protocol::decision_status blackboard::submit_decision(const acl::message&& decision) {
    return {};
}

} // namespace fabko