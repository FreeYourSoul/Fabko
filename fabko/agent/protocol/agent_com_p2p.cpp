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

#include "agent_com.hh"

namespace fabko::agent_protocol::p2p {

std::string agent_com::instantiate_black_board(const std::string&) {
    return {};
}

std::future<agent_protocol::propositions> agent_com::request_propositions(const std::string&) {
    return {};
}

} // namespace fabko
