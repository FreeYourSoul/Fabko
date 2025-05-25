// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 23/04/23. License 2022-2024
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#pragma once

#include <algorithm>
#include <memory>
#include <span>
#include <vector>

#include "common/logging.hh"

namespace fabko::sat {

/**
 * A specific result of a sat solver execution
 */
class sat_result {
public:
  explicit sat_result(std::vector<literal> data);

  [[nodiscard]] auto get() const {
      const auto size_positive = static_cast<std::size_t>(std::distance(
          _data.begin(), std::ranges::find_if(_data, [](const literal& lit) { return !static_cast<bool>(lit); })));

      return std::span{_data.data(), size_positive};
  }

  [[nodiscard]] auto get_all() const {
    return std::span{_data.data(), _data.size()};
  }

  friend std::string to_string(const sat_result&);

private:
  std::vector<literal> _data;
};

/**
 * Configuration of the solver
 */
struct solver_config {
  /**
   * Byte field representing the different flags that could be set at the solver level
   */
  struct {
    unsigned random_init : 1;
    unsigned multi_threaded : 1;
    unsigned status_solver : 3;
    unsigned previous_status : 3;
  } flags;

  double random_seed;
};

/**
 * Main solver class.
 *
 * This class is the entry point of the solver.
 */
class solver {
  struct sat_impl;

public:
  explicit solver(solver_config config);
  ~solver();

  solver(solver&&) noexcept;
  solver &operator=(solver&&) noexcept;

  // remove copy construction
  solver(const solver&) = delete;
  solver &operator=(const solver&) = delete;


  /**
   * Reset the context of the sat solver.
   * @details Set back the solver and discard the previous result.
   */
  void reset_solver();

  /**
   * Make it possible to continue the resolution of the solver. Starting from this point,
   * Adding var and clause are possible.
   * This method only works in case the Solver found a result (status SAT)
   * @details Set the solver as in the building phase. Does not discard the current result.
   */
  void reuse_solver();

  /**
   * Add a var in the SAT solver.
   * This step has to be done before calling the `solve` function
   *
   * @param number_to_add number of variables to add in the sat solver
   */
  void add_variables(std::size_t number_to_add);

  /**
   * Add a clause to the SAT solver
   * This step has to be done before calling the `solve` function
   *
   * @param clause_literals disjunction literals forming a clause (each clause is conjunctions)
   */
  void add_clause(clause clause_literals);

  /**
   * Start the resolution of the SAT solver.
   * From this point on, it is not possible to add clauses or var
   *
   * @param requested_sat_solution number of solutions to retrieve for the sat solver, -1 means all of them. defaulted to -1.
   */
  void solve(int requested_sat_solution = -1);

  /**
   * @return results got from the sat_solver
   */
  [[nodiscard]] std::vector<sat_result> results() const;

  /**
   * @return current status of the solver
   */
  [[nodiscard]] solver_status solving_status() const;

  /**
   * @return number of variables currently set in the SAT
   */
  [[nodiscard]] std::size_t nb_variables() const;

  /**
   * @return number of clauses currently set in the SAT
   */
  [[nodiscard]] std::size_t nb_clauses() const;

  /**
   * Convert the sat-solver into a dimacs file
   *
   * @param solver solver to convert
   * @param file_path destination file to store the resulting dimacs
   */
  friend void to_dimacs(const solver& solver, const std::string& file_path);

private:
  std::unique_ptr<sat_impl> _pimpl;
};

} // namespace fabko::sat