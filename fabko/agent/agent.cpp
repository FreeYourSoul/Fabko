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

#include <optional>
#include <thread>

#include <agent/protocol/acl.hh>

#include "agent.hh"

namespace fabko {

static constexpr std::size_t MAX_QUEUED_SIZE = 10;

agent::~agent() = default;

void agent_runner::run() {
    _runner_thread.request_stop();
    std::jthread([this](const std::stop_token& token) {
        while (!token.stop_requested()) {
            for (auto& agent : this->_agents) {
                agent.execute();
                std::this_thread::sleep_for(std::chrono::milliseconds {10});
            }
        }
    }).swap(_runner_thread);
}

void agent_runner::add_agent(agent&& agent) { _agents.push_back(std::move(agent)); }

void agent::execute() {
    // const auto msg_return = _callback_on_action(*this, std::nullopt);
    // for (const auto& msg : msg_return) {
    //     // todo : send message back
    // }
}

} // namespace fabko