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

#include "solver.hh"

namespace fabko::sat {

/**
 * Helper function to retrieve the literal v
 *
 * @param var var to get the assignment literal value
 * @param assignment assignment to have
 * @return an index (literal value) that a specific var would have for the given assignment
 */
[[nodiscard]] unsigned get_lit_index_for_variable(sat::variable var, bool assignment);

} // namespace fabko::sat