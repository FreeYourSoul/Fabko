// Dual Licensing Either:
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by fys on 06.04.26. @Copyright Licensing 2022-2026
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#ifndef FABKO_AST_HH
#define FABKO_AST_HH

#include "compiler/frontend/ir/fabl_ir.hh"
#include "compiler/metadata.hh"

namespace fabko::compiler::fabl::ast {

using resource = ir::resource;

struct resources_hardcoded {
    resource rsc;
};

struct actor_accessor {
    std::string actor;
    std::string value;
};

using accessor = std::variant<actor_accessor, std::string>;

struct constraint_op {
    ir::constraint_operation op;

    void str_to_op(const std::string& operation) {
        if (operation == "==") {
            op = ir::constraint_operation::EQUAL;
        } else if (operation == "!=") {
            op = ir::constraint_operation::DIFFERENT;
        } else if (operation == ">") {
            op = ir::constraint_operation::GREATER_THAN;
        } else if (operation == ">=") {
            op = ir::constraint_operation::GREATER_THAN_OR_EQUAL;
        } else if (operation == "<") {
            op = ir::constraint_operation::LESS_THAN;
        } else if (operation == "<=") {
            op = ir::constraint_operation::LESS_THAN_OR_EQUAL;
        }
    }
};

struct precondition {
    accessor lhs;
    constraint_op ope;
    accessor rhs;
};

struct effect_op {
    ir::operator_exec op;

    void str_to_op(char operation) {
        if (operation == '+') {
            op = ir::operator_exec::ADD;
        } else if (operation == '-') {
            op = ir::operator_exec::SUBTRACT;
        } else if (operation == '*') {
            op = ir::operator_exec::MULTIPLY;
        } else if (operation == '/') {
            op = ir::operator_exec::DIVIDE;
        } else if (operation == '%') {
            op = ir::operator_exec::MODULO;
        }
    }
};

struct effect {
    actor_accessor lhs;
    effect_op ope;
    std::string rhs;
};

struct capability {
    std::string name;
    std::vector<precondition> preconditions;
    std::vector<effect> effects;
};

struct actor {
    std::string name;

    resources_hardcoded resources;
};

struct fabl_program {
    std::vector<actor> actors;
    std::vector<resources_hardcoded> resources;
    std::vector<capability> capabilities;
};

} // namespace fabko::compiler::fabl::ast

#endif // FABKO_AST_HH
