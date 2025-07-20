// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 30/04/23. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
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
}

/**
 * @return true if all literals in the clause are set to a value that satisfies the clause, false otherwise
 */
bool is_clause_satisfied(const Solver_Context& ctx, const Clause& clause) {
    const auto& all_clause_lit = clause.get_literals();
    return std::ranges::any_of(all_clause_lit, [&ctx](const auto& lit) {
        const auto& [literal, varid] = lit;
        const auto assignment        = get<soa_assignment>(ctx.vars_soa_[varid]);
        return (literal.is_on() && assignment == assignment::on) || (literal.is_off() && assignment == assignment::off);
    });
}

bool has_conflict(const Solver_Context& ctx, const Clause& clause) { // @todo remove function when watchers is implemented
    const auto& all_clause_lit = clause.get_literals();
    return std::ranges::all_of(all_clause_lit, [&ctx](const auto& lit) {
        const auto& [literal, varid] = lit;
        const auto assignment        = get<soa_assignment>(ctx.vars_soa_[varid]);
        return (literal.is_on() && assignment == assignment::off) //
            || (literal.is_off() && assignment == assignment::on);
    });
}

/**
 * @brief Increase the VSIDS activity of the variables in the provided learned clause, then decrease the VSIDS activity of all variables if configured to do so.
 * @note this function is called after a conflict (and the learned clause from the conflict resolution is the parameter 'learned_clause')
 * @note if the VSIDS activity is too high, to avoid overflow, every variable activity is divided by 1e100
 * @param ctx solving context to update the VSIDS activity of the variables
 * @param learned_clause clause learned from the conflict resolution, the literals in this clause are used to increase the VSIDS activity of the variables
 */
void update_vsids_activity(Solver_Context& ctx, const Clause& learned_clause) {
    const auto& all_lit = learned_clause.get_literals();

    const bool need_normalization = std::ranges::any_of(all_lit, [&ctx](const auto& lit) { // check if any variable activity is too high
        const auto vsids_activity = get<soa_assignment_ctx>(ctx.vars_soa_[lit.second]).vsids_activity_;
        return vsids_activity >= (std::numeric_limits<decltype(vsids_activity)>::max() - ctx.config_.vsids_increment);
    });
    if (need_normalization) {
        static constexpr auto NORMALIZATION_FACTOR = 1e6;
        std::ranges::for_each(ctx.vars_soa_, fil::soa_select<soa_assignment_ctx>([](auto& assignment_ctx) { //
            assignment_ctx.vsids_activity_ /= NORMALIZATION_FACTOR;
        }));
    }

    // increase the VSIDS activity of the variables in the learned clause
    for (const auto& varid : all_lit | std::views::values) {
        auto& assignment_context = get<soa_assignment_ctx>(ctx.vars_soa_[varid]);
        assignment_context.vsids_activity_ += ctx.config_.vsids_increment;
    }

    // decrease the VSIDS for all variables if the counter of conflict exceeded the configured decay interval
    if (ctx.statistics_.conflicts % ctx.config_.decay_interval == 0) {
        std::ranges::for_each(ctx.vars_soa_, fil::soa_select<soa_assignment_ctx>([&ctx](auto& assignment_ctx) { //
            assignment_ctx.vsids_activity_ /= ctx.config_.vsids_decay_ratio;
        }));
    }
}

/**
 * @brief conflict resolution step consist of analyzing a found conflicting clause
 *  As the SAT solver implement CDCL (Clause-Driven Clause Learning) learned clause is retrieved from that and an indication of the backtracking to be done to
 *  continue SAT resolution
 * @param ctx solving context to resolve the conflict from
 * @param conflict_soastruct_clause clause that conflicted in the solving context
 * @return a resolution result that provides the learned clause as well as the backtracking level at which the solver must return to for continuation of the sat solve
 */
conflict_resolution_result resolve_conflict(Solver_Context& ctx, Clauses_Soa::soa_struct conflict_soastruct_clause) {
    const auto& [conflict_clause, watcher, meta] = conflict_soastruct_clause;
    log_debug("analyzing conflicting clause: {}", to_string(conflict_clause));

    auto current_level_vars = std::ranges::fold_left(conflict_clause.get_literals(),
        std::vector<Vars_Soa::soa_struct> {}, //
        [&ctx](std::vector<Vars_Soa::soa_struct> res, auto conflict_clausepair) {
            auto struct_soa            = ctx.vars_soa_[conflict_clausepair.second];
            const auto& assignment_ctx = get<soa_assignment_ctx>(struct_soa);
            if (assignment_ctx.decision_level_ == ctx.current_decision_level_) {
                res.push_back(struct_soa);
            }
            return res;
        });

    // learned clause to be returned : start as being equal to the conflict clause
    std::vector<std::pair<Literal, Vars_Soa::struct_id>> learned_clause = conflict_clause.get_literals();

    std::size_t backtrack_level = 0;                     // backtracking level retrieved from a learned clause
    std::size_t trail_index     = ctx.trail_.size() - 1; // index of the trail to backtrack to // @todo : do the iteration with reverse iterator

    // to find a UIP (Unique Implication Point), we need to resolve all the antecedent clause (clause resulting from the propagation of the literals)
    // at the current decision level, we need to keep one variable from the current level.
    while (current_level_vars.size() > 1) {

        while (trail_index > 0) {
            const auto trail_struct_id   = ctx.trail_[trail_index];
            const auto& trail_node       = ctx.vars_soa_[trail_struct_id];
            const auto trail_var         = get<soa_literal>(trail_node).value();
            const auto& trail_assign_ctx = get<soa_assignment_ctx>(trail_node);

            if (!trail_assign_ctx.is_decision() && trail_assign_ctx.decision_level_ == ctx.current_decision_level_) {
                if (std::ranges::any_of(current_level_vars, [trail_var](const auto& ss) { return get<soa_literal>(ss).value() == trail_var; })) {
                    break;
                }
            }

            --trail_index;
        }
        if (trail_index == 0) {
            log_debug(" :: trail empty");
            break;
        }

        const auto trail_struct_id   = ctx.trail_[trail_index];
        const auto& trail_node       = ctx.vars_soa_[trail_struct_id];
        const auto trail_lit         = get<soa_literal>(trail_node);
        const auto& trail_assign_ctx = get<soa_assignment_ctx>(trail_node);

        // remove trail literal found from the learned clause
        std::erase_if(learned_clause, [&trail_lit](const auto& lit_struct) { return get<soa_literal>(lit_struct).value() == trail_lit.value(); });

        log_debug(":: resolving with trail antecedent of {}", trail_lit.value());

        if (trail_assign_ctx.clause_propagation_.has_value()) {
            const auto& propagation_clausestruct = ctx.clauses_soa_[trail_assign_ctx.clause_propagation_.value()];
            const auto& propagation_vars         = get<soa_clause>(propagation_clausestruct).get_literals();

            // add literals from the propagation clause (except current trail literal)
            for (const auto [prop_lit, prop_varid] : propagation_vars) {
                if (prop_lit.value() == trail_lit.value()
                    || std::ranges::any_of(learned_clause, [&prop_lit](const auto& lit_map) { return prop_lit.value() == lit_map.first.value(); }))
                    continue;

                learned_clause.emplace_back(prop_lit, prop_varid);
                if (trail_assign_ctx.decision_level_ == ctx.current_decision_level_) {
                    current_level_vars.push_back(ctx.vars_soa_[prop_varid]);
                } else if (trail_assign_ctx.decision_level_ > backtrack_level) {
                    backtrack_level = trail_assign_ctx.decision_level_;
                }
            }
        }

        std::erase_if(current_level_vars, fil::soa_select<soa_literal>([trail_lit](auto lit_var) { return lit_var == trail_lit; }));

        log_debug(":: learning clause[{}] :: variable left current var level {}",
            std::ranges::fold_left(learned_clause, std::string {}, [](std::string&& res, const auto& lit) { return res + to_string(lit.first) + ", "; }),
            current_level_vars.size());

        --trail_index;
    }

    std::ranges::sort(learned_clause, [](const auto& lhs, const auto& rhs) { return lhs.first.value() < rhs.first.value(); });
    learned_clause.erase( //
        std::ranges::unique(learned_clause, [](const auto& lhs, const auto& rhs) { return lhs.first.value() == rhs.first.value(); }).begin(),
        learned_clause.end());

    log_debug("conflict resolution :: backtracking to level ({}) :: learned clause (clause[{}])",
        backtrack_level,                                                                                //
        std::ranges::fold_left(learned_clause, std::string {}, [](std::string&& res, const auto& lit) { //
            return res + ", " + to_string(lit.first);
        }));

    return {Clause {std::move(learned_clause)}, backtrack_level};
}

/**
 * @brief backtrack the SAT solver to the indicated level
 *  the SAT solver has a non-linear backtracking implementation. it is backtracking multiple levels to skip unnecessary decision branches thanks to the learned clause.
 * @param ctx of the sat solver
 * @param level to backtrack to
 */
void backtrack(Solver_Context& ctx, std::size_t level) {
    log_debug("backtracking start :: from {} to {}", ctx.current_decision_level_, level);
    while (!ctx.trail_.empty()) {
        const auto& node = ctx.trail_.back();
        auto soa_struct  = ctx.vars_soa_[node];

        auto& [literal, assignment, assignment_context, compiler_context] = soa_struct;

        if (assignment_context.decision_level_ <= level) {
            break;
        }
        assignment                             = assignment::not_assigned;
        assignment_context.clause_propagation_ = std::nullopt; // remove any propagation context from the assignment
        ctx.trail_.pop_back();
    }
    ctx.current_decision_level_ = level;
    log_debug("backtracking end :: backtracked to level {} :: size trail {}", level, ctx.trail_.size());
}

std::optional<Clauses_Soa::struct_id> unit_propagation(Solver_Context& ctx) {
    std::optional<Clauses_Soa::struct_id> conflict;
    [[maybe_unused]] auto propagate = [&](const auto& clause_struct) {
        if (conflict.has_value())
            return false;

        const auto& [clause, watchers, _] = clause_struct;

        if (is_clause_satisfied(ctx, clause)) {
            return false; // skip satisfied clause
        }

        const auto& clauselit_mapped_varid = clause.get_literals();
        const auto unassigned              = std::ranges::fold_left(clauselit_mapped_varid,
            std::vector<std::pair<Literal, Vars_Soa::struct_id>> {}, //
            [&ctx, &clause_struct](std::vector<std::pair<Literal, Vars_Soa::struct_id>> res, const auto& pair) {
                const auto clause_var_id                                    = pair.second;
                const auto& [literal, assignment, assignment_context, meta] = ctx.vars_soa_[clause_var_id];
                if (assignment != assignment::not_assigned) {
                    return res;
                }
                res.push_back(pair);
                return res;
            });

        if (unassigned.empty()) {
            // if (has_conflict(ctx, clause)) {
            conflict = clause_struct.struct_id(); // no literal is assigned : conflict detected
            log_debug("conflict found :: {}", to_string(clause));
            return false;
        }

        if (unassigned.size() == 1) {
            const auto& [unassigned_clauselit, unassigned_varid]  = unassigned.front();
            auto unassigned_soa_struct                            = ctx.vars_soa_[unassigned_varid];
            auto& [literal, assignment, assignment_context, meta] = unassigned_soa_struct;

            assignment                             = unassigned_clauselit.is_on() ? assignment::on : assignment::off; // set assignment of the propagation
            assignment_context.decision_level_     = ctx.current_decision_level_;                                     // set decision level of the propagation
            assignment_context.clause_propagation_ = clause_struct.struct_id(); // setup clause responsible for the propagation of the assignment
            ctx.trail_.push_back(unassigned_varid);                             // add the propagation in the trail

            log_debug("propagate decision: level({}) on {} :: {} -> {} ", assignment_context.decision_level_, to_string(clause), literal.value(), to_string(assignment));
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

void learn_additional_clause(Solver_Context& ctx, const Clause& clause_learned) {
    if (clause_learned.is_empty()) {
        log_debug("learned clause is empty, the solver is unsatisfiable", SECTION);
        return;
    }
    log_debug("learned clause: {}", to_string(clause_learned));
    [[maybe_unused]] const auto _ = ctx.clauses_soa_.insert(clause_learned, Clause_Watcher {ctx.vars_soa_, clause_learned}, Metadata {/*@todo: add compiler context from model*/});
    ++ctx.statistics_.learned_clause;
}

bool make_decision(Solver_Context& ctx) {
    auto unassigned_vars = //
        std::ranges::views::filter(ctx.vars_soa_, [](const auto& var) { return get<soa_assignment>(var) == assignment::not_assigned; });

    if (unassigned_vars.empty()) {
        log_debug("no unassigned variable found");
        return false;
    }

    auto var_highest_vsids = std::ranges::max_element(
        unassigned_vars, [](const auto& rhs, const auto& lhs) { return get<soa_assignment_ctx>(rhs).vsids_activity_ < get<soa_assignment_ctx>(lhs).vsids_activity_; });

    ++ctx.current_decision_level_;
    ctx.statistics_.max_decision_lvl = std::max(ctx.statistics_.max_decision_lvl, ctx.current_decision_level_);
    ++ctx.statistics_.decisions;

    auto s                                            = *var_highest_vsids;
    auto& [lit, assignment, assignment_context, meta] = s;

    assignment_context.decision_level_ = ctx.current_decision_level_;
    assignment                         = assignment::on; // always set at on by default. Off will be set by propagation (@incomplete : really ?)

    log_debug("make decision: level({}) :: {} -> {}", ctx.current_decision_level_, lit.value(), to_string(assignment));
    ctx.trail_.push_back((*var_highest_vsids).struct_id());

    return true;
}

std::expected<solver::result, sat_error> solve_sat(Solver_Context& ctx, const Model& model) {
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
            update_vsids_activity(ctx, learned_clause);

        } else {
            if (make_decision(ctx))
                continue;

            // no decision found, check if a solution is found
            const bool is_sat_solved = std::ranges::all_of(ctx.clauses_soa_, fil::soa_select<soa_clause>([&ctx](const auto& clause) { //
                return is_clause_satisfied(ctx, clause);
            }));
            if (is_sat_solved) {
                solution = std::ranges::fold_left(ctx.vars_soa_, solution, [](solver::result res, const auto& soa_struct) {
                    res.literals.emplace_back(                              //
                        get<soa_assignment>(soa_struct) == assignment::on ? //
                            get<soa_literal>(soa_struct).value() :
                            -get<soa_literal>(soa_struct).value());
                    return res;
                });
                log_info("solution found : {}", to_string(solution));
            }
        }
    }
    return solution;
}

} // namespace impl_details

Solver_Context::Solver_Context(const Model& model)
    : config_(model.conf)
    , model_(model)
    , vars_soa_([&]() {
        Vars_Soa vars;
        vars.reserve(model.literals.size());
        for (const auto& lit : model.literals) {
            [[maybe_unused]] auto _ =
                vars.insert(lit, assignment::not_assigned, Assignment_Context {/*empty assignment context*/}, Metadata {/*@todo: add compiler context from model*/});
        }
        return vars;
    }())
    , clauses_soa_([&]() {
        Clauses_Soa clauses;
        clauses.reserve(model.clauses.size());

        for (const std::vector<Literal>& model_clause : model.clauses) {
            auto all_clause_ids = std::ranges::fold_left(model_clause, std::vector<Vars_Soa::struct_id> {}, [&, this](auto res, const Literal& l) { //
                auto it = std::ranges::find_if(vars_soa_, fil::soa_select<soa_literal>([&, this](Literal lit) {                                     //
                    return lit == l;
                }));
                fabko_assert(it != vars_soa_.end(), "a clause cannot contains a non-defined literal");
                ++get<soa_assignment_ctx>(*it).vsids_activity_;
                res.push_back((*it).struct_id());
                return res;
            });

            Clause clause_to_insert {model_clause, std::move(all_clause_ids)};
            [[maybe_unused]] const auto _ = clauses.insert( //
                clause_to_insert,
                Clause_Watcher {vars_soa_, clause_to_insert},
                Metadata {/*@todo: add compiler context from model*/});
        }
        return clauses;
    }()) {}

} // namespace fabko::compiler::sat