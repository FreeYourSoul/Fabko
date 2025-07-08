//
// Created by Quentin on 25/05/2025.
//

#include <algorithm>
#include <expected>
#include <optional>
#include <ranges>
#include <set>

#include "common/exception.hh"
#include "common/logging.hh"
#include "solver.hh"

#include "solver_context.hh"

#include <numeric>

namespace fabko::compiler::sat {
namespace impl_details {

namespace {
constexpr std::string SECTION = "sat_solver"; //!< logging a section for the SAT solver
}

/**
 * @brief conflict resolution step consist of analysing a found conflicting clause
 *  As the SAT solver implement CDLC (Clause-Driven Learning Clause) learned clause is retrieved from that and an indication of the backtracking to be done to
 *  continue SAT resolution
 * @param ctx solving context to resolve the conflict from
 * @param conflict_soastruct_clause clause that conflicted in the solving context
 * @return a resolution result that provides the learned clause as well as the backtracking level at which the solver must return to for continuation of the sat solve
 */
conflict_resolution_result resolve_conflict(Solver_Context& ctx, Clauses_Soa::soa_struct conflict_soastruct_clause) {

    // , to_string(conflict_clause_id)
    log_debug("analyzing conflicting clause ");

    const auto& [conflict_clause, watcher, meta] = conflict_soastruct_clause; // ctx.clauses_soa_[conflict_clause_id];

    auto current_level_vars = std::ranges::fold_left(conflict_clause.vars(),
        std::vector<Vars_Soa::soa_struct> {},                                 //
        [&ctx](std::vector<Vars_Soa::soa_struct> res, auto conflict_clausepair) {
            auto struct_soa            = ctx.vars_soa_[conflict_clausepair.second];
            const auto& assignment_ctx = get<soa_assignment_ctx>(struct_soa);
            if (assignment_ctx.decision_level_ == ctx.current_decision_level_) {
                res.push_back(struct_soa);
            }
            return res;
        });

    // learned clause to be returned : start as being equal to the conflict clause
    std::vector<std::pair<literal, Vars_Soa::struct_id>> learned_clause = conflict_clause.vars();

    std::size_t backtrack_level = 0;                     // backtracking level retrieved from a learned clause
    std::size_t trail_index     = ctx.trail_.size() - 1; // index of the trail to backtrack to // @todo : do the iteration with reverse iterator

    // to find a UIP (Unique Implication Point), we need to resolve all the antecedent clause (clause resulting from the propagation of the literals)
    // at the current decision level, we need to keep one variable from the current level.
    while (current_level_vars.size() > 1) {

        while (trail_index > 0) {
            const auto trail_struct_id = ctx.trail_[trail_index - 1];
            const auto& trail_node     = ctx.vars_soa_[trail_struct_id];
            const auto var             = get<soa_literal>(trail_node).value();
            const auto& assignment_ctx = get<soa_assignment_ctx>(trail_node);

            if (!assignment_ctx.is_decision() && assignment_ctx.decision_level_ == ctx.current_decision_level_) {
                if (std::ranges::any_of(current_level_vars, [var](const auto& ss) { return get<soa_literal>(ss).value() == var; })) {
                    break;
                }
            }

            --trail_index;
        }
        if (trail_index == 0)
            break;

        const auto trail_struct_id   = ctx.trail_[trail_index];
        const auto& trail_node       = ctx.vars_soa_[trail_struct_id];
        const auto trail_lit         = get<soa_literal>(trail_node);
        const auto& trail_assign_ctx = get<soa_assignment_ctx>(trail_node);

        // remove trail literal found from the learned clause
        std::erase_if(learned_clause, [&trail_lit](const auto& lit_struct) { return get<soa_literal>(lit_struct).value() == trail_lit.value(); });

        if (trail_assign_ctx.clause_propagation_.has_value()) {
            const auto& propagation_clausestruct = ctx.clauses_soa_[trail_assign_ctx.clause_propagation_.value()];
            const auto& propagation_vars         = get<soa_clause>(propagation_clausestruct).vars();

            for (const auto [prop_lit, prop_varid] : propagation_vars) {
                if (prop_lit.value() == trail_lit.value()
                    || std::ranges::none_of(learned_clause, //
                        [&prop_lit](const auto& lit_map) { return prop_lit.value() == lit_map.first.value(); }))
                    continue;

                learned_clause.emplace_back(prop_lit, prop_varid);
                auto found_in_trail = std::ranges::any_of(ctx.trail_, //
                    [&prop_varid](auto trail_id) { return prop_varid == trail_id; });
                if (found_in_trail) {
                    current_level_vars.push_back(ctx.vars_soa_[prop_varid]);
                } else {
                    backtrack_level = trail_assign_ctx.decision_level_;
                }
            }
        }

        std::erase_if(current_level_vars, fil::soa_select<soa_literal>([trail_lit](auto lit_var) { return lit_var == trail_lit; }));

        --trail_index;
    }

    std::ranges::sort(learned_clause, [](const auto& a, const auto& b) { return a.first.value() < b.first.value(); });
    learned_clause.erase(std::ranges::unique(learned_clause).begin(), learned_clause.end());

    return {clause {std::move(learned_clause)}, backtrack_level};
}

/**
 * @brief backtrack the SAT solver to the indicated level
 *  The SAT solver has a non linear backtracking implementation. It is backtracking multiple levels to not skip un-necessary decision branches thanks to the learned clause.
 * @param ctx of the sat solver
 * @param level to backtrack to
 */
void backtrack(Solver_Context& ctx, std::size_t level) {
    log_debug("backtracking start :: from {} to {}", ctx.current_decision_level_, level);
    while (ctx.trail_.empty()) {
        const auto& node = ctx.trail_.back();
        auto soa_struct  = ctx.vars_soa_[node];

        auto& [literal, assignment, assignment_context, compiler_context] = soa_struct;

        if (assignment_context.decision_level_ <= level) {
            break;
        }
        assignment = assignment::not_assigned;
        ctx.trail_.pop_back();
    }
    ctx.current_decision_level_ = level;
}

std::optional<Clauses_Soa::struct_id> unit_propagation(Solver_Context& ctx) {

    std::optional<Clauses_Soa::struct_id> conflict;
    [[maybe_unused]] auto propagate = [&](const auto& clause_struct) {
        if (conflict.has_value())
            return false;

        const auto& [clause, assignment, _] = clause_struct;
        const auto& clauselit_mapped_varid  = clause.vars();
        const auto unassigned               = std::ranges::fold_right(clauselit_mapped_varid,
            std::vector<std::pair<literal, Vars_Soa::struct_id>> {}, //
            [&ctx, &clause_struct](const auto& pair, auto res) {
                const auto clause_var_id                                    = pair.second;
                const auto& [literal, assignment, assignment_context, meta] = ctx.vars_soa_[clause_var_id];
                if (assignment != assignment::not_assigned) {
                    return res;
                }
                res.push_back(pair);
                return res;
            });

        if (unassigned.empty()) {
            conflict = clause_struct.struct_id(); // no literal is assigned, this clause is a conflict
            return false;
        }
        if (unassigned.size() == 1) {
            const auto& [unassigned_clauselit, unassigned_varid]  = unassigned.front();
            auto unassigned_soa_struct                            = ctx.vars_soa_[unassigned_varid];
            auto& [literal, assignment, assignment_context, meta] = unassigned_soa_struct;

            assignment                             = unassigned_clauselit.is_on() ? assignment::on : assignment::off; // set assignment of the propagation
            assignment_context.clause_propagation_ = clause_struct.struct_id(); // setup clause responsible for the propagation of the assignment
            ctx.trail_.push_back(unassigned_varid);                             // add the propagation in the trail
            return true;
        }
        return false;
    };

    // execute propagation unit there is no propagation happening
    // propagate return the number of propagations that occurred on a single loop over the clauses, this is repeated in case of a cascade effect.
    while (auto propagated_literal = std::ranges::count_if(ctx.clauses_soa_, propagate)) {
        if (conflict.has_value()) {
            break;
        }
        ctx.statistics_.propagations += propagated_literal;
    }
    return conflict;
}

void learn_additional_clause(Solver_Context& ctx, const clause& clause) {
    if (clause.is_empty()) {
        log_debug("learned clause is empty, the solver is unsatisfiable", SECTION);
        return;
    }
    // @todo learn clause
    // ctx.clauses_soa_.
    ++ctx.statistics_.learned_clause;
}

bool make_decision(Solver_Context& ctx) {

    auto var_highest_vsids = std::ranges::max_element(ctx.vars_soa_, //
        [](const auto& lhs, const auto& rhs) { return get<soa_assignment_ctx>(lhs).vsids_activity_ < get<soa_assignment_ctx>(rhs).vsids_activity_; });

    if (var_highest_vsids != ctx.vars_soa_.end()) {
        log_info("no decision found");
        return false;
    }

    ++ctx.current_decision_level_;
    ctx.statistics_.max_decision_lvl = std::max(ctx.statistics_.max_decision_lvl, ctx.current_decision_level_);
    ++ctx.statistics_.decisions;

    auto& [lit, assignment, assignment_context, meta] = *var_highest_vsids;

    assignment = assignment::on;

    ctx.trail_.push_back((*var_highest_vsids).struct_id());

    return true;
}

std::expected<solver::result, sat_error> solve_sat(Solver_Context& ctx, const model& model) {
    if (unit_propagation(ctx).has_value())
        return std::unexpected(sat_error::unsatisfiable);

    solver::result solution;
    while (solution.literals.empty()) {
        if (ctx.conflict_count_since_last_restart_ >= ctx.config_.restart_threshold) {
            ctx.conflict_count_since_last_restart_ = 0;
            ++ctx.statistics_.restarts;

            // Restart by backtracking to decision level 0
            backtrack(ctx, 0);

            // Update the restart threshold for the next restart
            ctx.config_.restart_threshold *= static_cast<int>(ctx.config_.restart_multiplier);
        }

        const auto conflict = unit_propagation(ctx);
        if (conflict.has_value()) {
            ++ctx.conflict_count_since_last_restart_;
            ++ctx.statistics_.conflicts;
            const auto& [learned_clause, backtrack_level] = resolve_conflict(ctx, ctx.clauses_soa_[conflict.value()]);

            if (learned_clause.is_empty()) {
                log_info("Conflict resolved into an empty clause, unsatisfiable");
                return std::unexpected(sat_error::unsatisfiable);
            }
            learn_additional_clause(ctx, learned_clause);
            backtrack(ctx, backtrack_level);
        } else {
            make_decision(ctx);
        }
    }
    return solution;
}

} // namespace impl_details

Solver_Context::Solver_Context(const model& model)
    : config_(model.conf)
    , model_(model)
    , vars_soa_([&]() {
        Vars_Soa vars;
        vars.reserve(model.literals.size());
        for (const auto& lit : model.literals) {
            [[maybe_unused]] auto _ =
                vars.insert(lit, assignment::not_assigned, assignment_context {/*empty assignment context*/}, metadata {/*@todo: add compiler context from model*/});
        }
        return vars;
    }())
    , clauses_soa_([&]() {
        Clauses_Soa clauses;
        clauses.reserve(model.clauses.size());

        for (const std::vector<literal>& model_clause : model.clauses) {
            auto all_clause_ids = std::ranges::fold_right(model_clause, std::vector<Vars_Soa::struct_id> {}, [&, this](const literal& l, auto res) { //
                auto it = std::ranges::find_if(vars_soa_, fil::soa_select<soa_literal>([&, this](literal lit) {                                      //
                    return lit == l;
                }));
                fabko_assert(it != vars_soa_.end(), "a clause cannot contains a non-defined literal");
                res.push_back((*it).struct_id());
                return res;
            });

            clause clause_to_insert {model_clause, std::move(all_clause_ids)};
            [[maybe_unused]] const auto _ = clauses.insert( //
                clause_to_insert,
                clause_watcher {vars_soa_, clause_to_insert},
                metadata {/*@todo: add compiler context from model*/});
        }
        return clauses;
    }()) {}

} // namespace fabko::compiler::sat