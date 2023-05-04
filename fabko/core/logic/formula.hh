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
  std::string token;
};

class formula;
using expression = std::variant<std::shared_ptr<formula>, std::shared_ptr<const formula>, variable>;

namespace op {

struct conjunction {};
struct disjunction {};
struct at_least_one {};

using operand = std::variant<
    at_least_one,
    conjunction,
    disjunction>;
} // namespace op

constexpr op::conjunction conjunction{};
constexpr op::disjunction disjunction{};
constexpr op::at_least_one at_least_one{};

class formula : std::enable_shared_from_this<formula> {

public:
  formula(expression lhs, op::operand op, expression rhs)
      : _lhs(std::move(lhs)), _rhs(std::move(rhs)), _op(op) {}

  [[nodiscard]] std::string express_cnf_string() const;
  [[nodiscard]] std::vector<sat::literal> express_cnf_literals() const;


  expression &get_lhs();
  const expression &get_lhs() const;

  expression &get_rhs();
  const expression &get_rhs() const;

  op::operand &get_op();
  const op::operand &get_op() const;

private:
  expression _lhs;
  expression _rhs;
  op::operand _op;
};

static auto make_formula(expression lhs, const op::operand& op, expression rhs) {
  return std::make_shared<formula>(std::move(lhs), op, std::move(rhs));
}

} // namespace fabko::logic
