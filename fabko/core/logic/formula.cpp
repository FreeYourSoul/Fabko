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
#include <ranges>
#include <sat/solver.hh>

#include "common/exception.hh"
#include "common/visitor_utils.hh"
#include "formula.hh"

namespace fabko::logic {

namespace {

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

std::string express_formula_string(const formula_ptr& form) {
    std::string res;
    visit(
        form,
        overloaded{
            [&res](auto&&, visit_op_enter_flag) { res += "("; },
            [&res](auto&&, visit_op_exit_flag) { res += ")"; },
            [&res](auto&& f, visit_var_flag) { res += f.token; },
            [&res](auto&& f, visit_op_flag) {
                std::visit(overloaded{
                               [&res](op::conjunction) { res += " ∧ "; },
                               [&res](op::disjunction) { res += " ∨ "; }},
                           f->get_op());
            },
            [](auto&&, auto&&) {}});
    return res;
}

auto apply_tseytin_transformation(const formula_ptr& form) {
    std::vector<sat::clause> res;
    visit(
        form,
        overloaded{
            [&res](auto&&, visit_op_enter_flag) {},
            [&res](auto&&, visit_op_exit_flag) {},
            [&res](auto&& f, visit_var_flag) {},
            [&res](auto&& f, visit_op_flag) {
                std::visit(overloaded{
                               [&res](op::conjunction) {},
                               [&res](op::disjunction) {}},
                           f->get_op());
            },
            [](auto&&, auto&&) {}});
    return res;
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

set::set(std::size_t var_number)
    : _set_var(var_number) {
    for (std::size_t i = 0; i < var_number; ++i) {
        _set_var[i].token = fmt::format("{}", i);
    }
}

variable& set::operator[](unsigned index) {
    return _set_var[index];
}

std::span<variable> set::operator[](unsigned index_begin, unsigned index_end) {
    fabko_assert(index_begin < index_end,
                 fmt::format("operator[] error : index_begin ({}) should be smaller that index_end ({})", index_begin, index_end));
    fabko_assert(index_end <= _set_var.size(),
                 fmt::format("operator[] error : index_begin ({}) and index_end ({}) should be in acceptable range of the set", index_begin, index_end));
    return {
        _set_var.begin() + static_cast<long>(index_begin),
        _set_var.begin() + static_cast<long>(index_end)};
}

variable& conj(variable& var, std::vector<std::string> token) {
    return var;
}

variable& disj(variable& var, std::vector<std::string> token) {
    return var;
}

std::span<variable> conj(std::span<variable> vars, std::vector<std::string> token) {
    return vars;
}

std::span<variable> disj(std::span<variable> vars, std::vector<std::string> token) {
    return vars;
}
} // namespace fabko::logic