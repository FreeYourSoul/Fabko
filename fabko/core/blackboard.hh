// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2023
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

#include "boardcom.hh"

namespace fabko {

struct blackboard_data {
    std::string id{};
    com::request initial_request;
    com::propositions propositions{};
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

    template<com::c_board_com BoardCommunication>
    explicit blackboard(BoardCommunication&& bc, com::request initial_request);

    [[nodiscard]] com::propositions request_propositions(const com::request& request);
    [[nodiscard]] com::decision_status submit_decision(const std::string& decision);

  private:
    std::unique_ptr<blackboard_impl> _pimpl;
};

} // namespace fabko
