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
#include <memory>
#include <expected>

namespace fabko {

template<typename action>
class agent {
    struct impl;

  public:
    ~agent() = default;

    // extract requirement of the type on a static assertion as agent symbol is not available to the compiler at the
    // template declaration level.
    static_assert(
        std::is_invocable_v<action, agent&>,
        "The action function provided to the agent has to be with the following signature: void(agent&)");


    [[nodiscard]] std::expected<>

  private:
    void execute_action() {
        _callback_on_action(*this);
    }

    action _callback_on_action;

    std::unique_ptr<impl> _pimpl;

};

} // namespace fabko