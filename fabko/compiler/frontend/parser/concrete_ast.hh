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
#include <fil/datastructure/soa.hh>

//@note cst stands for Concrete Syntax Tree

namespace fabko::compiler::fabl::cst {
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

using identifier = std::string;

struct custom_data_type;
struct capability;
struct capability_identifier;
struct has_statement;

using null_type       = std::monostate;
using literal_integer = std::int32_t;
using literal_string  = std::string;
using literal_bool    = bool;

template<typename T> struct named_member {
    std::string name;
    T value;
};

using data_type = std::variant<    //
    null_type,                     //
    custom_data_type,              //
    has_statement,
    capability,                    //
    capability_identifier,
    named_member<literal_bool>,    //
    named_member<literal_integer>, //
    named_member<literal_string>>;

enum class compare_operator {
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    EQUAL,
    DIFFERENT,
};

enum class visibility {
    PRIVATE,
    PUBLIC
};

// when a type (

enum class compile_status {
    DEFINED,  //!< complete type (defined properly in a compilation unit)
    DECLARED, //!< incomplete type for now (declared only -- expected to be defined at a later stage)
};

enum class compile_unit_type {
    FILE,
    RAW
};

enum class mutability {
    CONSTANT,
    MUTABLE,
    MUTABLE_IN_SCOPE
};

struct outcome {};

struct condition {
    std::shared_ptr<data_type> lhs;
    compare_operator op;
    std::shared_ptr<data_type> rhs;
};

struct prerequisites {
    std::vector<condition> conditions;
};

struct capability {
    prerequisites pre;
    outcome post;
};

struct capability_identifier {
    identifier id;
};

struct has_statement {
    identifier id;
    std::int32_t quantity;
};

struct custom_data_type {
    std::string name;
    std::vector<data_type> content;
};

struct compile_unit_id {
    std::string name;
};

struct metadata_info {
    compile_unit_id unit;
    compile_unit_type type;
    std::size_t line_definition;
};

struct symbol_table {
    fil::soa::soa<identifier, //
        compile_status, mutability, visibility, metadata_info, std::unique_ptr<data_type>>
        structure;
};

struct fabl_program {
    fil::soa::soa<compile_unit_id, //
        symbol_table>
        structure;
};

} // namespace fabko::compiler::fabl::cst

#endif // FABKO_AST_HH
