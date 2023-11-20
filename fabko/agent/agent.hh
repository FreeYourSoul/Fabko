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
#include <expected>
#include <functional>
#include <memory>

namespace fabko {

class agent {
    struct impl;

  public:
    template<typename action>
        requires std::is_invocable_v<action, agent&>
    explicit agent(action func) : _callback_on_action(std::move(func)) {}

    ~agent() = default;

    //    [[nodiscard]] std::expected<>;

  private:
    void execute_action() {
        _callback_on_action(*this);
    }

    std::function<void(agent&)> _callback_on_action;

    std::unique_ptr<impl> _pimpl;
};

} // namespace fabko