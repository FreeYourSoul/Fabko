// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <concepts>
#include <future>
#include <optional>
#include <string>
#include <vector>

#include "protocol/agent_com.hh"
#include "protocol/fap_request.hh"

namespace fabko {

struct blackboard_data {
    agent_protocol::request initial_request;
    agent_protocol::propositions propositions{};
};

/**
 *
 * @tparam BoardCommunication
 */
class blackboard {

  private:
    struct blackboard_impl;

  public:
    ~blackboard();

    template<agent_protocol::c_communicate BoardCommunication>
    explicit blackboard(BoardCommunication&& bc, agent_protocol::request initial_request);

    [[nodiscard]] agent_protocol::propositions request_propositions(const acl::message& request);
    [[nodiscard]] agent_protocol::decision_status submit_decision(const acl::message&& decision);

  private:
    std::unique_ptr<blackboard_impl> _pimpl;
};

} // namespace fabko
