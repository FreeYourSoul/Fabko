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

namespace fabko::sat {

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

class clause {

private:

public:

};

/**
 * context of a current sat resolution
 */
struct context {

};

/**
 * configuration of the solver
 */
struct solver_config {

  using custom_allocator_literal = std::false_type;
  using custom_allocator_clause = std::false_type;

  /**
   * byte field representing the different flags that could be set at the solver level
   */
  struct {
    unsigned random_init: 1;
    unsigned multi_threaded: 1;
  } flags;
  double random_seed;
};

class solver {
  struct impl;

public:
  explicit solver(solver_config config);
  ~solver();

private:
  std::unique_ptr<impl> _pimpl;
};

}