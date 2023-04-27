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
using variable_watched = std::pair<variable, assign_bool>;

namespace {
struct sat_execution_context {
  assignment_bitset<> cur_assignment{};
  std::vector<assignment_bitset<>> valid_assignments{};
};

/**
   * Watcher structure used for the temporary cur_assignment and backtracking of the SAT solver algorithm.
   */
struct clause_watchers {
  clause clause;
  /**
     * Variable currently watched in the on-going algorithm.
     */
  variable_watched watch;
};

}// namespace

struct solver::sat_impl {

public:
  explicit sat_impl(fabko::sat::solver_config config) : config(config) {
  }

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
        context.valid_assignments.emplace_back(context.cur_assignment);
        continue;
      }

      const auto assignment_happened = try_assign_variable(attempt_flags, var);

      // nothing has been attempted :: backtracking
      if (!assignment_happened) {
        // variable cannot continue backtracking. Solver is in error if no result has been previously found.
        if (var == 0) {
          return context.valid_assignments.empty() ? solver_status::UNSAT : solver_status::SAT;
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
   * Update the watcher list in accordance with the assign value.
   *
   * @details: For each literals, clause are watching them. When updating the watchlist for a specific variable
   *
   * @param var
   * @param assign
   * @return
   */
  bool update_watchlist(sat::variable var, bool assign) {
  }

public:
  solver_config config;
  std::unordered_map<unsigned, std::vector<clause>> clauses{};
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
//  ...
}

solver_status solver::solving_status() const {
  return solver_status(_pimpl->config.flags.status_solver);
}

void solver::solve(int requested_sat_solution) {
  _pimpl->config.flags.status_solver = unsigned(solver_status::SOLVING);
  _pimpl->config.flags.status_solver = unsigned(_pimpl->solve_sat(requested_sat_solution));
}

}// namespace fabko::sat
