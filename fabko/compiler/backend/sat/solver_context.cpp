//
// Created by Quentin on 25/05/2025.
//

#include <algorithm>
#include <expected>
#include <optional>
#include <ranges>

#include "common/exception.hh"
#include "common/logging.hh"
#include "solver.hh"

#include "solver_context.hh"

#include <numeric>

namespace fabko::compiler::sat {
namespace impl_details {

namespace {

constexpr std::string SECTION = "sat_solver"; //!< logging a section for the SAT solver

conflict_resolution_result resolve_conflict(clause_soa::struct_id conflict_clause_id) {
    // clause learned_clause(conflict_clause.vars());
    std::size_t backtrack_level = 0;

    // find UIP : Unique Implication Point
    // return {std::move(learned_clause), backtrack_level};
    return {clause({}), backtrack_level};
}

bool backtrack(solver_context& ctx, std::size_t level) {
    while (ctx.trail_.empty()) {
        const auto& node = ctx.trail_.back();
    }
    return true;
}

std::optional<clause_soa::struct_id> unit_propagation(solver_context& ctx) {

    std::optional<clause_soa::struct_id> conflict;
    [[maybe_unused]] auto propagate = [&](const auto& clause_struct) {
        if (conflict.has_value())
            return false;

        const auto& [clause, assignment, _]                   = clause_struct;
        const std::vector<var_soa::struct_id>& clause_var_ids = clause.vars();
        const std::vector<literal> literal_unassigned         = std::ranges::fold_right(clause_var_ids, std::vector<literal> {}, [&ctx](var_soa::struct_id var_id, auto res) {
            const auto& [literal, assignment, assignment_context, compiler_context] = ctx.vars_soa_[var_id];
            if (assignment != assignment::off) {
                return res;
            }
            // res.push_back(literal);
            // ctx.trail_.push_back(s.);
            return res;
        });

        if (literal_unassigned.empty()) {
            conflict = clause_struct.struct_id(); // no literal is assigned, this clause is a conflict
            return false;
        }
        if (literal_unassigned.size() == 1) {
            return true;
        }
        return true;
    };
    while (auto propagated_literal = std::ranges::count_if(ctx.clauses_soa_, propagate)) {
        if (conflict.has_value()) {
            break;
        }
        ctx.statistics_.propagations += propagated_literal;
    }
    return conflict;
}

void learn_additional_clause(solver_context& ctx, const clause& clause) {
    // ctx.clauses_soa_.push_back(clause);
    if (clause.is_empty()) {
        log_debug("learned clause is empty, the solver is unsatisfiable", SECTION);
        return;
    }
}
} // namespace

std::expected<solver::result, sat_error> solve_sat(solver_context& ctx, const model& model) {
    if (!unit_propagation(ctx))
        return std::unexpected(sat_error::unsatisfiable);

    solver::result solution;
    while (solution.literals.empty()) {
        if (ctx.conflict_count_since_last_restart_ >= ctx.config_.restart_threshold) {
            ctx.conflict_count_since_last_restart_ = 0;
            ++ctx.statistics_.restarts;

            // Restart by backtracking to decision level 0
            backtrack(ctx, 0);

            // Update restart threshold for next restart
            ctx.config_.restart_threshold *= static_cast<int>(ctx.config_.restart_multiplier);
        }

        const auto conflict = unit_propagation(ctx);
        if (conflict.has_value()) {
            ++ctx.conflict_count_since_last_restart_;
            ++ctx.statistics_.conflicts;
            const auto& [learned_clause, backtrack_level] = resolve_conflict(ctx.clauses_soa_[conflict.value()].struct_id());

            if (learned_clause.is_empty()) {
                log_info("Conflict resolved into an empty clause, unsatisfiable");
                return std::unexpected(sat_error::unsatisfiable);
            }
            learn_additional_clause(ctx, learned_clause);
            backtrack(ctx, backtrack_level);
        }
    }
    return solution;
}

} // namespace impl_details

solver_context::solver_context(const model& model)
    : config_(model.conf)
    , model_(model)
    , vars_soa_([&]() {
        var_soa vars;
        vars.reserve(model.literals.size());
        for (const auto& lit : model.literals) {
            [[maybe_unused]] auto _ =
                vars.insert(lit, assignment::not_assigned, assignment_context {/*empty assignment context*/}, compiler_context {/*@todo: add compiler context from model*/});
        }
        return vars;
    }())
    , clauses_soa_([&]() {
        clause_soa clauses;
        clauses.reserve(model.clauses.size());

        for (const std::vector<literal>& model_clause : model.clauses) {
            auto all_clause_ids = std::ranges::fold_right(model_clause, std::vector<var_soa::struct_id> {}, [&, this](const literal& l, auto res) { //
                auto it = std::ranges::find_if(vars_soa_, fil::soa_select<soa_literal>([&, this](const literal& lit) { return lit == l; }));
                fabko_assert(it != vars_soa_.end(), "a clause cannot contains a non-defined literal");
                res.push_back((*it).struct_id());
                return res;
            });

            clause clause_to_insert {std::move(all_clause_ids)};
            [[maybe_unused]] const auto _ = clauses.insert( //
                clause_to_insert,
                clause_watcher {vars_soa_, clause_to_insert},
                compiler_context {/*@todo: add compiler context from model*/});
        }
        return clauses;
    }()) {}

} // namespace fabko::compiler::sat