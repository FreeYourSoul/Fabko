// MIT License
//
// Created by FyS on 03/04/23. License 2022-2023
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
//         of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
//         to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//         copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
//         copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//         AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <utility>
#include <vector>

#include <sat/assignment_bitset.hh>

#include "solver.hh"

namespace fabko::sat {

//
// SAT Solver implementation details
//

using clause = std::vector<literal>;

struct variable_watched {
  variable v{};
  std::optional<bool> assign{};
};

namespace {
struct sat_execution_context {
  /**
   * Assignment currently in trial for satisfiability.
   */
  assignment_bitset<> cur_assignment{};

  /**
   * Assignment that has been registered as being potential result for the SAT resolution.
   * If this vector is filled with at least one value, it means the SAT resolved.
   *
   * It is possible for the SAT solver to resolve as many time as required (or until no new resolution is found).
   * It depends on the configuration of the SAT solver before calling `resolve`
   */
  std::vector<assignment_bitset<>> valid_result_assignments{};
};

}// namespace

struct solver::sat_impl {

public:
  explicit sat_impl(fabko::sat::solver_config config)
      : config(config) {}

  /**
   * Entry point of the SAT solver algorithm.
   *
   * @return status of the sat solver :: could be either SAT ( Satisfiable ) or UNSAT ( UnSatisfiable )
   */
  [[nodiscard]] solver_status solve_sat(int requested_sat_solution) {
    const std::size_t num_var = context.cur_assignment.nb_vars();

    // The list of binary flag wil keep track of what values for which variables  we have tried so far.
    // 0 means nothing has been tried yet
    // 1 means False has been tried but not True
    // 2 means True but not False
    // 3 means both have been tried
    // bitwise operation over the value 1 or 0 is made to set the flag
    std::vector<int> attempt_flags{};
    attempt_flags.resize(num_var);

    std::size_t var = 0;

    // var is the variable to set
    while (requested_sat_solution > 0 || requested_sat_solution == -1) {

      // if at the end of the variable count, initiate backtracking
      if (var == num_var) {
        if (requested_sat_solution == -1) {
          --requested_sat_solution;
        }
        --var;
        // store current cur_assignment result (as it is a valid result)
        context.valid_result_assignments.emplace_back(context.cur_assignment);
        continue;
      }

      const auto assignment_happened = try_assign_variable(attempt_flags, var);

      // nothing has been attempted :: backtracking
      if (!assignment_happened) {
        // variable cannot continue backtracking. Solver is in error if no result has been previously found.
        if (var == 0) {
          return context.valid_result_assignments.empty() ? solver_status::UNSAT : solver_status::SAT;
        }

        // reset attempts
        attempt_flags[var] = 0;
        context.cur_assignment.unassign_variable(sat::variable(var));
        --var;

      }
      // if an assignment has been attempted. Go forward to the next variable;
      else {
        ++var;
      }
    }

    return solver_status::SAT;
  }

private:
  /**
   * attempt an assignment on false and then true.
   * if already attempted : the flag is not retried and backtracking should be initiated
   *
   * @param attempt_flags flags of attempts (0 no attempts 1 false attempt, 2 true attempt, 3 false and true attempt)
   * @param var variable to try an assignment
   * @return true if assignment occurred, false otherwise
   */
  [[nodiscard]] bool try_assign_variable(std::vector<int>& attempt_flags, std::size_t var) {
    for (const bool attempt : {false, true}) {
      if ((attempt_flags[var] >> attempt) & 1) {
        continue;
      }
      // set the bit for the attempt on the variable var.
      attempt_flags[var] |= 1 << static_cast<unsigned>(attempt);

      // apply assignment
      context.cur_assignment.assign_variable(sat::variable(var), attempt);

      if (update_watchlist(sat::variable(var), attempt)) {
      }

      return true;
    }
    return false;
  }

  /**
   * Update the watcher list in accordance with the assigned value (we look for all clauses which are watching the opposite of the assignment).
   *
   * @details:
   *    For each literals, clause are watching them. When updating the watchlist for a specific variable
   *    we go through all the clause watching the updated variable. An alternative is looked for each of the clause.
   *
   * @param var variable to trigger update of the watch list
   * @param assign assignment happening on the variable. Retrieve the watcher of the opposite of the assignment.
   * @return True if assignment is possible and a proper alternative has been found for each clause watching the variable.
   *    False otherwise (if false is returned then assignment is contradicting the resolvability and a backtracking should start)
   */
  [[nodiscard]] bool update_watchlist(sat::variable var, bool assign) {
    auto& clauses_watching = watchlist.at((~literal{var, assign}).value());

    auto rm_when_find_alternative =
        clauses_watching
        | std::ranges::views::take_while([this](const auto& clause_watching) {
            return std::ranges::any_of(clause_watching.get(), [this, &clause_watching](const literal& lit) {
              const variable alt = lit.variable();// alternative to check
              if (!context.cur_assignment.is_assigned(alt) || context.cur_assignment.is(alt, static_cast<bool>(lit))) {
                watchlist[alt].emplace_back(clause_watching);
                return true;
              }
              return false;
            });
          });

    clauses_watching.erase(
        std::begin(clauses_watching),
        std::begin(clauses_watching) + std::ranges::distance(rm_when_find_alternative));

    if (!clauses_watching.empty()) {
      // all alternatives have not been found :: return false assignment is contradicting a clause.
      return false;
    }
    // alternative has been found for every single one in the watching list
    return true;
  }

public:
  solver_config config;
  std::vector<clause> clauses{};
  /**
   * watch list per literal for clauses
   */
  std::unordered_map<unsigned, std::vector<std::reference_wrapper<const clause>>> watchlist{};
  sat_execution_context context{};
};

//
// SAT Solver public API
//

solver::~solver() = default;

solver::solver(fabko::sat::solver_config config)
    : _pimpl(std::make_unique<sat_impl>(std::move(config))) {
  reset_solver();
}

void solver::reset_solver() {
  _pimpl->context = {};
  _pimpl->config.flags.status_solver = unsigned(solver_status::BUILDING);
  _pimpl->config.flags.previous_status = unsigned(solver_status::BUILDING);
}

void solver::reuse_solver() {
  _pimpl->config.flags.previous_status = _pimpl->config.flags.status_solver;
  _pimpl->config.flags.status_solver = unsigned(solver_status::BUILDING);
}

void solver::add_variables(std::size_t number_to_add) {
  fabko_assert(solving_status() == solver_status::BUILDING,
               "cannot add a new variable in the SAT solver if the solver is not in building phase");
  _pimpl->context.cur_assignment.reserve_new_variable(number_to_add);
}

void solver::add_clause(clause clause_literals) {
  fabko_assert(solving_status() == solver_status::BUILDING,
               "cannot add a new clause in the SAT solver if the solver is not in building phase");

  fabko_assert(!clause_literals.empty(),
               "an added clause cannot be empty");

  auto watcher = variable_watched{clause_literals[0].variable(), std::nullopt};

  _pimpl->clauses.emplace_back(std::move(clause_literals));
  for (const auto& lit : _pimpl->clauses.back()) {
    _pimpl->watchlist[lit.variable()].emplace_back(_pimpl->clauses.back());
  }
}

solver_status solver::solving_status() const {
  return solver_status(_pimpl->config.flags.status_solver);
}

void solver::solve(int requested_sat_solution) {
  _pimpl->config.flags.status_solver = unsigned(solver_status::SOLVING);
  _pimpl->config.flags.status_solver = unsigned(_pimpl->solve_sat(requested_sat_solution));
}

}// namespace fabko::sat
