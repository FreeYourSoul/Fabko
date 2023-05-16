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

#pragma once

#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// forward declarations
namespace fabko::sat {
class literal;
}

namespace fabko::logic {

struct variable {

    friend variable &conj(std::span<std::string>& token);
    friend variable &disj(variable&);

    std::string token;
};

class formula;
using expression = std::variant<std::shared_ptr<formula>, std::shared_ptr<const formula>, variable>;

namespace op {

struct conjunction {};
struct disjunction {};

using operand = std::variant<
    conjunction,
    disjunction>;
} // namespace op

constexpr op::conjunction conjunction{};
constexpr op::disjunction disjunction{};

class formula;
using formula_ptr = std::shared_ptr<formula>;

class formula : public std::enable_shared_from_this<formula> {

  public:
    formula(expression lhs, op::operand op, expression rhs)
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)), _op(op) {}

    friend std::string express_formula_string(const formula_ptr& form);
    friend auto apply_tseytin_transformation(const formula_ptr& form);

    expression get_lhs() const;
    expression get_rhs() const;
    op::operand get_op() const;

  private:
    expression _lhs;
    expression _rhs;
    op::operand _op;
};

class set {
  public:
    explicit set(std::size_t var_number);

    variable &operator[](unsigned index);
    std::span<variable> operator[](unsigned index_begin, unsigned index_end);

  private:
    std::vector<variable> _set_var;
};

static auto make_formula(expression lhs, op::operand op, expression rhs) {
    return std::make_shared<formula>(std::move(lhs), std::move(op), std::move(rhs));
}

} // namespace fabko::logic
