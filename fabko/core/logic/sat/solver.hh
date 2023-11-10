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

#include <algorithm>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <vector>

namespace fabko::sat {

/**
 * Variable in the CNF representation.
 * This value is an index into the memory object and thus is not abstracted.
 */
using variable = unsigned;

/**
 * Represent a literal in the CNF representation.
 *
 * The value of the literal is equivalent to the var it represent mult by 2
 *  +1 if the var is positive
 *  +0 if the var is negative
 */
class literal {

public:
  literal(variable var, bool positive) : _val(var + var + static_cast<unsigned>(positive)) {}

  bool operator<=>(const literal& other) const = default;

  literal operator~() const {
    auto tmp = literal(*this);
    tmp._val ^= 1;
    return tmp;
  }

  //! true or false depending on current assignment designation of the literal
  explicit operator bool() const {
    auto v = _val;
    return (v & 1) == 1;
  }

  [[nodiscard]] variable var() const {
    auto v = _val;
    return v >> 1;
  };

  [[nodiscard]] unsigned value() const { return _val; };

  friend std::string to_string(const literal&);

private:
  unsigned _val;
};

enum class solver_status : unsigned {
  BUILDING = 0,
  SOLVING  = 1,
  SAT      = 2,
  UNSAT    = 3,
};

/**
 * A clause is a set of literals linked by disjunction (∨)
 */
using clause = std::vector<literal>;

/**
 * A specific result of a sat solver execution
 */
class sat_result {
public:
  explicit sat_result(std::vector<literal> data);

  [[nodiscard]] auto get() const {
    const auto size_positive = std::size_t(
        std::distance(_data.begin(), std::ranges::find_if(_data, [](const literal& lit) { return !bool(lit); })));

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
   * @details Set the solver as in building phase. Does not discard the current result.
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
   * @param clause_literals disjunction literals forming a clause (each individual clause are conjunctions)
   */
  void add_clause(clause clause_literals);

  /**
   * Start the resolution of the SAT solver.
   * From this point on, it is not possible to add clauses or var
   *
   * @param requested_sat_solution number of solution to retrieve for the sat solver, -1 means all of them. defaulted to -1.
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
   * @return number of variable currently set in the SAT
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