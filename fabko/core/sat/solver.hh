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

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

namespace fabko::sat {

/**
 * Leveraged boolean in order to represent a negation - non-negation - non-cur_assignment of a var
 */
using assign_bool = std::optional<bool>;

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
    return (unsigned(_val) & 1) == 1;
  }

  [[nodiscard]] variable var() const { return unsigned(_val) >> 1; };
  [[nodiscard]] unsigned value() const { return _val; };

private:
  unsigned _val;
};

enum class solver_status : unsigned {
  BUILDING = 0,
  SOLVING = 1,
  SAT = 2,
  UNSAT = 3,
};

/**
 * Configuration of the solver
 */
struct solver_config {

  using custom_allocator_literal = std::false_type;
  using custom_allocator_clause = std::false_type;

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
 *
 * @copyright
 *   This solver is based on the algorithm developed for [minisat](https://github.com/niklasso/minisat) project and is based
 *   on their released paper on their [website](http://minisat.se/).   None if not little of those algorithm is developed for
 *   the Fabko project.
 */
class solver {
  struct sat_impl;

public:
  explicit solver(solver_config config);
  ~solver();

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
  void add_clause(std::vector<literal> clause_literals);

  /**
   * Start the resolution of the SAT solver.
   * From this point on, it is not possible to add clauses or var
   *
   * @param requested_sat_solution number of solution to retrieve for the sat solver, -1 means all of them.
   */
  void solve(int requested_sat_solution = 1);

  /**
   * @return current status of the solver
   */
  [[nodiscard]] solver_status solving_status() const;

private:
  std::unique_ptr<sat_impl> _pimpl;
};

}// namespace fabko::sat