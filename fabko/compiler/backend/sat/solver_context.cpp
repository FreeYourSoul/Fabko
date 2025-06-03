//
// Created by Quentin on 25/05/2025.
//

#include <algorithm>
#include <optional>

#include "solver_context.hh"

#include "common/exception.hh"
#include "solver.hh"

#include <iostream>

namespace fabko::compiler::sat {

solver_context::solver_context(const model& model)
    : clauses_watcher_([&model]() { //
          std::vector<impl_details::clause_watcher> watchers;
          watchers.reserve(model.clauses.size());
          std::ranges::transform(model.clauses, std::back_inserter(watchers), //
                                 [](const auto& clause) { return impl_details::clause_watcher(clause); });
          return watchers;
      }()),
      conflict_count_(0),         //
      current_decision_level_(0), //
      config_(model.conf) {}

namespace impl_details {

class clause_watcher {
  public:
    explicit clause_watcher(const clause& clause)
        : clause_ref_(clause), watchers_([&clause]() -> std::vector<literal_view> { //
              fabko_assert(clause.literals().empty(), "Cannot make a clause watchers over an empty clause");
              if (clause.literals().size() == 1) {
                  return {clause.literals().front()};
              }
              return {clause.literals().front(), clause.literals().back()};
          }()) {}

  private:
    std::reference_wrapper<const clause> clause_ref_;
    std::vector<literal_view> watchers_;
};

class solver_solution {
  public:
  private:
    std::vector<literal_assigned> literals_;
};
/**
 * @brief Represents a literal assignment in an SAT solver with its related context.
 *
 * This class maintains the state of an assigned literal in the SAT solver, including
 * its decision level, VSIDS activity score, and the reason for its assignment
 * (Whether it was a decision or propagated through a clause.)
 *
 */
class literal_assigned {
  public:
    enum class type_assign {
        decision,
        propagated,
    };

    [[nodiscard]] bool is_decision() const;
    [[nodiscard]] bool is_propagated() const;

  private:
    literal_view literal_ref_;    //!< literal assigned
    std::uint8_t assignment_;     //!< assignment made on 2 bit (0 no attempt, 1 true attempted, 2 false attempted)
    std::int64_t vsids_activity_; //!< VSIDS (Variable State Independent Decaying Sum) activity value type
    std::size_t decision_level_;  //!< decision level of the literal

    std::optional<clause_view> clause_propagation_; //!< clause that produced this (nullopt if decision type)
};

struct conflict_resolution_result {
    clause learned_clause;       //!< clause learned from conflict resolution
    std::size_t backtrack_level; //!< level the conflict resolution found to requires the solver to backtrack to
};

conflict_resolution_result resolve_conflict(const clause& conflict_clause) {
    clause learned_clause(conflict_clause.literals());
    std::size_t backtrack_level = 0;

    return {std::move(learned_clause), backtrack_level};
}

bool literal_assigned::is_decision() const { return !clause_propagation_.has_value(); }
bool literal_assigned::is_propagated() const { return clause_propagation_.has_value(); }

std::optional<solver::result> solve_sat(solver_context& ctx, const model& model) {
    std::optional<solver::result> solved;
    while (!solved.has_value()) {
        if (ctx.conflict_count_ >= ctx.config_.restart_threshold) {

            // Restart by backtracking to decision level 0
            backtrack(0);

            // Update restart threshold for next restart
            ctx.config_.restart_threshold *= static_cast<int>(ctx.config_.restart_multiplier);
            restarts++;
        }

        bool conflict = unit_propagation();
        if (conflict) {
            ++ctx.conflict_count_;
            conflict_resolution_result res = resolve_conflict(conflict_clause);
            learned_clause                 = res.learned_clause;
            backtrack_level                = res.backtrack_level;
        }
    }
    return solved;
}

} // namespace impl_details
} // namespace fabko::compiler::sat