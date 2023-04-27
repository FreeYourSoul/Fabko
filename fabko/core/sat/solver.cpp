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

#include <common/ranges_to.hh>
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

    // var is the var to set
    sat::variable var = 1;

    while (requested_sat_solution > 0 || requested_sat_solution == -1) {

      // if at the end of the var count, initiate backtracking
      if (var > num_var) {
        fmt::print("DEBUG -- solve -- init backtracking - var :: {} - num_var :: {}\n", var, num_var);
        --var;
        continue;
      }

      const bool assignment_happened = try_assign_variable(attempt_flags, var);

      // nothing has been attempted :: backtracking
      if (!assignment_happened) {
        // var cannot continue backtracking. Solver is in error if no result has been previously found.
        if (var == 0) {
          fmt::print("DEBUG -- SAT execution -- end");

          // store current cur_assignment result (as it is a valid result)
          context.valid_result_assignments.emplace_back(context.cur_assignment);
          if (requested_sat_solution > 0) {
            --requested_sat_solution;
          }
          return context.valid_result_assignments.empty() ? solver_status::UNSAT : solver_status::SAT;
        }

        // reset attempts
        attempt_flags[var] = 0;
        context.cur_assignment.unassign_variable(var);
        fmt::print("DEBUG -- solve -- unassigned :: {}\n", var);

        --var;

      }
      // if an assignment has been attempted. Go forward to the next var;
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
   * @param var variable to try an assignment on
   * @return true if assignment occurred, false otherwise
   */
  [[nodiscard]] bool try_assign_variable(std::vector<int>& attempt_flags, sat::variable var) {
    for (const bool attempt : {true, false}) {
      if ((attempt_flags[var] >> attempt) & 1) {
        continue;
      }
      // set the bit for the attempt on the var.
      attempt_flags[var] |= 1 << static_cast<unsigned>(attempt);

      fmt::print("DEBUG -- solve::try_assign_variable -- var :: {} - attempt :: {}\n", var, attempt);
      // apply assignment
      context.cur_assignment.assign_variable(var, attempt);
      const bool watch_list_update_success = update_watchlist(var, attempt);

      fmt::print("DEBUG -- solve::try_assign_variable -- assign var::{}[{}] - success :: {} \n\n", var, attempt, watch_list_update_success);
      if (watch_list_update_success) {
        return true;
      }
      context.cur_assignment.unassign_variable(var);
    }
    fmt::print("DEBUG -- solve::try_assign_variable -- var :: {} - no assignment\n", var);
    return false;
  }

  /**
   * Update the watcher list in accordance with the assigned value (we look for all clauses which are watching the opposite of the assignment).
   *
   * @details:
   *    For each literals, clause are watching them. When updating the watchlist for a specific var
   *    we go through all the clause watching the updated var. An alternative is looked for each of the clause.
   *
   * @param var var to trigger update of the watch list
   * @param assign assignment happening on the var. Retrieve the watcher of the opposite of the assignment.
   * @return True if assignment is possible and a proper alternative has been found for each clause watching the var.
   *    False otherwise (if false is returned then assignment is contradicting the resolvability and a backtracking should start)
   */
  [[nodiscard]] bool update_watchlist(sat::variable var, bool assign) {
    auto opposite_literal = ~literal{var, assign};
    auto it_found = watchlist.find(opposite_literal.value());
    if (it_found == watchlist.end()) {
      return true;
    }
    auto& clauses_watching = it_found->second;
    fmt::print("DEBUG -- solve::update_watchlist -- var assign :: {}[{}] - over {} clause :: {}\n",
               var, assign, clauses_watching.size(),
               fmt::join(clauses_watching
                             | std::ranges::views::transform([this](std::size_t index){ return clauses[index];})
                             | std::ranges::views::transform([](const auto &lits){
                                std::string res{};
                                for (const auto& lit : lits) {
                                  res += fmt::format(" lit::{}[{}] -", lit.var(), bool(lit));
                                }
                                 return res;})
                             ,
                         "|"));

    auto rm_when_find_alternative =
        clauses_watching
        | std::ranges::views::transform([this](std::size_t index_clause) {
            return std::pair<std::size_t, const clause&>(index_clause, clauses[index_clause]);
          })
        | std::ranges::views::take_while([this](const auto& index_with_clause) {
            const auto& [index_clause, clause_watching] = index_with_clause;

            return std::ranges::any_of(clause_watching, [this, &index_clause](const literal& lit) {
              const variable alt = lit.var();// alternative to check
              fmt::print("DEBUG -- solve::update_watchlist::take_while::any_of clause {} -- lit::{}[{}] -- (check_alternative) ", index_clause, alt, bool(lit));
              fmt::print("-- is_assigned(alt)::{} -- is(alt, bool(alt))::{} \n", context.cur_assignment.is_assigned(alt), context.cur_assignment.is(alt, bool(lit)));
              if (!context.cur_assignment.is_assigned(alt) || context.cur_assignment.is(alt, bool(lit))) {
                watchlist[alt].emplace_back(index_clause);
                return true;
              }
              return false;
            });
          });

    const auto number_to_remove = std::ranges::distance(rm_when_find_alternative);
    fmt::print("DEBUG -- solve::update_watchlist -- removal of elements in watchlist of {} : remove::{} ", clauses_watching.size(), number_to_remove);
    clauses_watching.erase(
        std::begin(clauses_watching),
        std::begin(clauses_watching) + number_to_remove);
    fmt::print("-- after removal {}\n ", clauses_watching.size());

    if (!clauses_watching.empty()) {
      // all alternatives have not been found :: return false assignment is contradicting a clause.

      // debug print the contradicted clause
      fmt::print("INFO  -- clause contradicted assign var::{}[{}] -- ", var, assign);
      for (std::size_t c : clauses_watching) {

        for (const auto& lit : clauses[c])
          fmt::print("lit::{}[{}], ", lit.var(), bool(lit));

        fmt::print("<< clause {}\n", c);
      }

      return false;
    }
    // alternative has been found for every single one in the watching list
    return true;
  }

public:
  solver_config config;
  std::vector<clause> clauses{};
  /**
   * watch list per literal for clauses ::
   * key   <unsigned>                 -> is the value of the literal
   * value <std::vector<std::size_t>> -> is the list of index in the clause vector that are watching the literal
   */
  std::unordered_map<unsigned, std::vector<std::size_t>> watchlist{};
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
               "cannot add a new var in the SAT solver if the solver is not in building phase");
  fmt::print("DEBUG -- add_variables :: number_to_add {}\n", number_to_add);
  _pimpl->context.cur_assignment.reserve_new_variable(number_to_add);
}

void solver::add_clause(clause clause_literals) {
  fabko_assert(solving_status() == solver_status::BUILDING,
               "cannot add a new clause in the SAT solver if the solver is not in building phase");

  fabko_assert(!clause_literals.empty(),
               "an added clause cannot be empty");

  auto watcher = variable_watched{clause_literals[0].var(), std::nullopt};

  _pimpl->clauses.push_back(std::move(clause_literals));
  fmt::print("DEBUG -- add_clause -- clause added size :: {}\n", std::size(_pimpl->clauses.back()));
  for (const auto& lit : _pimpl->clauses.back()) {
    fmt::print("DEBUG -- watchlist add -- lit.value :: {}\n", lit.value());
    _pimpl->watchlist[lit.value()].emplace_back(_pimpl->clauses.size() - 1);
    _pimpl->watchlist[(~lit).value()].emplace_back(_pimpl->clauses.size() - 1);
  }

  for (const auto& [key, watched] : _pimpl->watchlist) {
    fmt::print("======================================\n");

    fmt::print("DEBUG -- watchlist content :: lit.value::{}\n", key);
    for (const auto& watched_indexes : watched) {
      const auto& clause = _pimpl->clauses[watched_indexes];
      fmt::print("DEBUG -----------------:: clauses (size - {})\n", clause.size());

      for (const auto& lit : clause) {
        fmt::print("var::{}[{}] (v:{}) -- ", lit.var(), bool(lit), lit.value());
      }
      fmt::print("\n");
    }
    fmt::print("======================================\n");
  }
}

solver_status solver::solving_status() const {
  return solver_status(unsigned(_pimpl->config.flags.status_solver));
}

void solver::solve(int requested_sat_solution) {
  fmt::print("DEBUG -- solve -- requested_solutions :: {}\n", requested_sat_solution);
  _pimpl->config.flags.status_solver = unsigned(solver_status::SOLVING);
  _pimpl->config.flags.status_solver = unsigned(_pimpl->solve_sat(requested_sat_solution));
}

std::vector<std::vector<literal>> solver::results() const {
  std::vector<std::vector<literal>> res;
  res.reserve(_pimpl->context.valid_result_assignments.size());
  for (const auto& result : _pimpl->context.valid_result_assignments) {
    auto lit_res = fabko::ranges::to_vector(
        std::ranges::views::iota(sat::variable{1}, result.nb_vars() + 1)
        | std::views::transform([&result](sat::variable var) {
            return literal{var, result.is_true(var)};
          }));

    res.emplace_back(std::move(lit_res));
  }
  return res;
}

}// namespace fabko::sat
