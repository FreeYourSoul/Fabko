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

#include "agent_com.hh"

namespace fabko::agent_protocol::p2p {

std::string agent_com::instantiate_blackboard(const acl::message&) {
    return {};
}

std::future<agent_protocol::propositions> agent_com::request_propositions(const acl::message&) {
    return {};
}
decision_status agent_com::commit_decision(const acl::message&) {
    return decision_status::RETRY;
}

} // namespace fabko
