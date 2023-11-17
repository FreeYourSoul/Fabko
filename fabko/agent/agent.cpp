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

#include <array>

#include "agent.hh"

#include "protocol/fap_request.hh"

namespace fabko {

constexpr std::size_t MAX_QUEUED_SIZE = 10;

//! implementation details of the agent
struct impl {


    /**
     * list of actions that the agent is going to do. The list is ordered by priority
     */
    std::array<agent_protocol::request, MAX_QUEUED_SIZE> action_list;  // implementation as a ring buffer may prove beneficial

};

}