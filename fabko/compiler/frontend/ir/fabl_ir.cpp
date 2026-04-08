// Dual Licensing Either:
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by fys on 07.04.26. @Copyright Licensing 2022-2026
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <nlohmann/json.hpp>

#include "fabl_ir.hh"

namespace fabko::compiler::fabl::ir {

NLOHMANN_JSON_SERIALIZE_ENUM(operator_exec, //
    {
        {operator_exec::ADD,      "add"      },
        {operator_exec::SUBTRACT, "substract"},
        {operator_exec::DIVIDE,   "divide"   },
        {operator_exec::MODULO,   "modulo"   },
        {operator_exec::MULTIPLY, "multitply"},
});

NLOHMANN_JSON_SERIALIZE_ENUM(constraint_operation, //
    {
        {constraint_operation::EQUAL,                 "equal"                },
        {constraint_operation::GREATER_THAN,          "greater than"         },
        {constraint_operation::GREATER_THAN_OR_EQUAL, "greater or equal than"},
        {constraint_operation::LESS_THAN,             "less than"            },
        {constraint_operation::LESS_THAN_OR_EQUAL,    "less or equal than"   },
})

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(actor_id, name_);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(capability, name, actor, target, loc);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(effect, lhs, op, rhs, loc);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(constraint, lhs, operation, rhs, loc);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(program, actors, resources, capabilities);

} // namespace fabko::compiler::fabl::ir