// Dual Licensing Either :
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
#include <stdexcept>
#include <variant>

#include <fmt/format.h>
#include <sat/solver.hh>

#include "common/visitor_utils.hh"
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
struct visit_op_enter_flag {};
struct visit_op_exit_flag {};
struct visit_op_flag {};
struct visit_var_flag {};
constexpr visit_var_flag visit_var{};
constexpr visit_op_flag visit_op{};
constexpr visit_op_exit_flag visit_op_exit{};
constexpr visit_op_enter_flag visit_op_enter{};

using visit_flag = std::variant<visit_op_flag, visit_op_enter_flag, visit_op_exit_flag, visit_var_flag>;

template<std::invocable<expression, visit_flag> Callable>
void visit(expression f, Callable&& handler) {
  auto* cur = std::get_if<std::shared_ptr<formula>>(&f);

  if (cur == nullptr) {
    handler(std::get<variable>(f), visit_var);
    return;
  }

  // callback entering the formula
  handler((*cur), visit_op_enter);

  visit((*cur)->get_lhs(), handler);
  handler((*cur), visit_op);
  visit((*cur)->get_rhs(), handler);

  // callback exiting the formula
  handler((*cur), visit_op_exit);
}

} // namespace

std::string formula::express_cnf_string() {
  std::string res;
  visit(
      shared_from_this(),
      overloaded{
          [&res](auto, visit_op_enter_flag) {
            res += "(";
          },
          [&res](std::shared_ptr<formula> f, visit_op_flag) {
            std::visit(
                overloaded{
                    [&res]<typename T>(T&&) {
                      if constexpr (std::same_as<T, op::conjunction>) {
                        res += " ∧ ";
                      }
                      if constexpr (std::same_as<T, op::disjunction>) {
                        res += " ∨ ";
                      }
                    }},
                f->get_op());
          },
          [&res](auto, visit_op_exit_flag) {
            res += ")";
          },
          [&res](variable f, visit_var_flag) {
            res += f.token;
          },
          [](auto, auto) {
            throw std::runtime_error("should not happens");
          }});
  return res;
}

std::vector<sat::literal> formula::express_cnf_literals() const {
  return {};
}

expression formula::get_lhs() const {
  return _lhs;
}

expression formula::get_rhs() const {
  return _rhs;
}

op::operand formula::get_op() const {
  return _op;
}

} // namespace fabko::logic