// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 13/11/2023. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <variant>

#include <protocol/fap_request.hh>

namespace fabko {

enum class action_source {
    automatic,
    manual
};

/**
 * Represent an action that an agent is doing / going to do.
 * This is
 */
struct agent_action {

    unsigned action_id{};

    action_source source{action_source::automatic};

    agent_protocol::request initial_request;

    agent_protocol::status status;
};

} // namespace fabko