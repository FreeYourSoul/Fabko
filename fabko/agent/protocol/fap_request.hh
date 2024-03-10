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

#include <string>
#include <variant>
#include <vector>

namespace fabko::agent_protocol {

struct lesser_than {};
struct greater_than {};
struct greater_or_eq {};
struct lesser_or_eq {};
struct eq {};

//! all possible comparator
using comparator = std::variant<lesser_or_eq, lesser_than, greater_than, greater_or_eq, eq>;

enum class status : char {
    awaiting = 0,
    on_going = 1,
    done     = 2,
    wont_do  = 3,
    cancel   = 4
};

struct resource {
    std::string resource;

    auto operator<=>(const struct resource&) const = default;
};

//! option are filters that can restrict a specific request, the importance weight on the comparison importance
struct option {
    resource resource_id;
    unsigned importance{};
    comparator cmp;
};

struct request {
    std::string emitter;
    std::string request;

    // this vector is ordered by importance before usage in order to ensure that the most valuable filter is first in the list
    // of responses.
    std::vector<option> options;
};

} // namespace fabko::agent_protocol