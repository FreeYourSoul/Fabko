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

#include <utility>


#include "solver.hh"

namespace fabko::sat {

using clause = std::vector<literal>;
using variable_watched = std::pair<variable, assign_bool>;

namespace {
struct sat_execution_context {
  assignement_bitset assignment;
};
}// namespace

/**
 * Implementation details of the solver class
 */
struct solver::impl {

  struct clause_watchers {
    clause clause;
    std::vector<variable_watched> watch;
  };

  std::vector<clause_watchers> clauses;
  std::vector<> ;

  solver_config config;
};

solver::solver(fabko::sat::solver_config config)
    : _pimpl(std::make_unique<impl>(std::move(config))) {}

solver::~solver() = default;

}// namespace fabko::sat
