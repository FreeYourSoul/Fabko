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

#pragma once

#include <concepts>
#include <expected>
#include <functional>
#include <memory>
#include <optional>
#include <thread>

namespace fabko {

// forward declaration
namespace acl {
struct message;
}

/**
 * @brief agent
 */
class agent {
    struct impl;

  public:
    template<typename action>
        requires std::is_invocable_v<action, agent&, std::optional<fabko::acl::message>>
    explicit agent(action func) : _callback_on_action(std::move(func)) {}
    ~agent();

    void execute();

  private:
    std::function<std::vector<fabko::acl::message>(agent&, std::optional<fabko::acl::message>)> _callback_on_action;
};

/**
 * @brief agent_runner
 */
class agent_runner {

  public:
    void run();
    void add_agent(agent&& agent);

  private:
    std::jthread _runner_thread;
    std::vector<agent> _agents;
};

} // namespace fabko