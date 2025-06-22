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

#include "solver.hh"

#include <algorithm>
#include <optional>

namespace fabko::compiler::sat {

namespace impl_details {
std::optional<solver::result> solve_sat(solver_context& ctx, const model& model);
} // namespace impl_details

solver::solver(model m)
    : context_(m)
    , model_(std::move(m)) {}

std::vector<solver::result> solver::solve(std::int32_t expected) {
    std::vector<result> res;

    for (auto i = 0; i < expected; ++i) {
        auto r = impl_details::solve_sat(context_, model_);
        if (!r.has_value()) {
            break;
        }
        res.push_back(std::move(r.value()));

        // add a constraint to disable the found solution
    }

    return res;
}

} // namespace fabko::compiler::sat