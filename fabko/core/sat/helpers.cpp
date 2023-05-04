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

#include "helpers.hh"

namespace fabko::sat {

unsigned get_lit_index_for_variable(variable var, bool assignment) {
    return (var << 1) | static_cast<unsigned>(assignment);
}

} // namespace fabko::sat