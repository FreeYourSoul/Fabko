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

using effect_ast_node = fil::copa::ast_node<[](const std::string& token) -> ir::operator_exec {
    if (token == "+")
        return ir::operator_exec::ADD;
    if (token == "-")
        return ir::operator_exec::SUBTRACT;
    if (token == "*")
        return ir::operator_exec::MULTIPLY;
    if (token == "/")
        return ir::operator_exec::DIVIDE;
    if (token == "%")
        return ir::operator_exec::MODULO;

    return ir::operator_exec::INVALID;
}>;

using precondition_ast_node = fil::copa::ast_node<[](const std::string& token) -> ir::constraint_operation {
    if (token == "==")
        return ir::constraint_operation::EQUAL;
    if (token == "!=")
        return ir::constraint_operation::DIFFERENT;
    if (token == ">")
        return ir::constraint_operation::GREATER_THAN;
    if (token == ">=")
        return ir::constraint_operation::GREATER_THAN_OR_EQUAL;
    if (token == "<")
        return ir::constraint_operation::LESS_THAN;
    if (token == "<=")
        return ir::constraint_operation::LESS_THAN_OR_EQUAL;

    return ir::constraint_operation::INVALID;
}>;

struct capability {
    std::string name;
    std::vector<precondition_ast_node> preconditions;
    std::vector<effect_ast_node> effects;
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
