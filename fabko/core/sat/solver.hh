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

#include <optional>
#include <cstdint>
#include <memory>

namespace fabko::sat {

/**
 * Leveraged boolean in order to represent a negation - non-negation - non-assignment of a variable
 */
using assign_bool = std::optional<bool>;

/**
 * Variable in the CNF representation.
 * This value is an index into the memory object and thus is not abstracted.
 */
using variable = int;

/**
 * Represent a literal in the CNF representation.
 *
 * The value of the literal is equivalent to the variable it represent mult by 2
 *  +0 if the variable is positive
 *  +1 if the variable is negated
 */
class literal {

public:
  literal(variable var, bool positive) : _val(var + var + static_cast<std::int32_t>(positive)){}

  literal operator^(bool b) const {
    literal lit(*this);
    lit._val ^= static_cast<std::int32_t>(b);
    return lit;
  }

  literal operator~() const {
    literal lit(*this);
    lit._val ^= 1;
    return lit;
  }
private:
  std::int32_t _val;

};

/**
 * Context of a current sat resolution
 */
struct context {
};

struct sat_execution_context {
  //  std::vector<v>
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
    unsigned random_init: 1;
    unsigned multi_threaded: 1;
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

  void add_variables(int number_to_add);
  void add_clause(std::vector<literal> clause_literals);

private:
  std::unique_ptr<sat_impl> _pimpl;
};

}