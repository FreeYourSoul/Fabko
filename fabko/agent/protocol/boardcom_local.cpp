// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.

#include <agent.hh>

#include "boardcom.hh"

namespace fabko::agent_protocol::local {

struct board_protocol::impl {
    std::vector<std::shared_ptr<agent>> _agent_ring;
};

board_protocol::~board_protocol() = default;

std::string board_protocol::instantiate_black_board(const std::string&) {
    return {};
}

std::future<propositions> board_protocol::request_propositions(const std::string&) {
    return std::future<propositions>();
}

agent_protocol::decision_status board_protocol::commit_decision(const std::string&) {
    return agent_protocol::decision_status::RETRY;
}

board_protocol::board_protocol(): _impl(std::make_unique<impl>()) {
}

}