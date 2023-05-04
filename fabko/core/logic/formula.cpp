`` // Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 03/05/23. License 2022-2023
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <stack>

#include <fmt/format.h>
#include <sat/solver.hh>

#include "formula.hh"

    namespace fabko::logic {

  namespace {

  template<typename T>
  bool is_variable(T&) {
    if constexpr (std::same_as<T, variable>) {
      return true;
    } else {
      return false;
    }
  }

  template<std::invocable<formula&> Callable>
  void visit(expression f, Callable&& handler) {
    while(!is_variable(f)) {
      auto& cur = std::get<std::shared_ptr<formula>>(f);

      visit(cur->get_lhs(), handler);
      visit(cur->get_rhs(), handler);
      handler(*cur);
    }
  }

  } // namespace

  std::string formula::express_cnf_string() const {
    std::string res;
    visit(shared_from_this(), [](formula& f) {
      
    });
    return res;
  }

  std::vector<sat::literal> formula::express_cnf_literals() const {
    return {};
  }

  expression& formula::get_lhs() {
    return _lhs;
  }

  const expression& formula::get_lhs() const {
    return _lhs;
  }

  expression& formula::get_rhs() {
    return _lhs;
  }

  const expression& formula::get_rhs() const {
    return _lhs;
  }

  op::operand& formula::get_op() {
    return _op;
  }

  const op::operand& formula::get_op() const {
    return _op;
  }

} // namespace fabko::logic``