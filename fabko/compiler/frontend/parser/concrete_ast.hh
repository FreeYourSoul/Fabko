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

using identifier = std::string;

struct custom_data_type;
struct capability;
struct capability_identifier;
struct has_statement;

using null_type       = std::monostate;
using literal_integer = std::int32_t;
struct literal_string {
    std::string value;
};

struct literal_bool {
    bool value;
};

enum class operator_exec : int {
    NONE,
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
};

enum class compare_operator : int {
    INVALID,
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

struct resources_hardcoded {
    resource rsc;
};

struct actor_accessor {
    std::string actor;
    std::optional<std::string> access = std::nullopt;
};

using precondition_ast_node = fil::copa::ast_node<[](const std::string& token) -> compare_operator {
    if (token == "==")
        return compare_operator::EQUAL;
    if (token == "!=")
        return compare_operator::DIFFERENT;
    if (token == ">")
        return compare_operator::GREATER;
    if (token == ">=")
        return compare_operator::GREATER_EQUAL;
    if (token == "<")
        return compare_operator::LESS;
    if (token == "<=")
        return compare_operator::LESS_EQUAL;

    return compare_operator::INVALID;
},
    actor_accessor>;

template<typename T> struct named_lit {
    std::string name;
    T value;
};

using expression_node = fil::copa::ast_node<[](const std::string& token) -> operator_exec {
    if (token == "+")
        return operator_exec::ADD;
    if (token == "-")
        return operator_exec::SUBTRACT;
    if (token == "*")
        return operator_exec::MULTIPLY;
    if (token == "/")
        return operator_exec::DIVIDE;
    if (token == "%")
        return operator_exec::MODULO;

    return operator_exec::NONE;
},
    literal_bool, actor_accessor>;

using data_type = std::variant< //
    null_type,                  //
    custom_data_type,           //
    has_statement,              //
    capability,                 //
    capability_identifier,      //
    literal_integer,            //
    literal_string,             //
    literal_bool,               //
    named_lit<literal_bool>,    //
    named_lit<literal_integer>, //
    named_lit<literal_string>>;

struct assignment;
struct outcomes {
    std::vector<assignment> assignments;
};

struct prerequisites_conjunction {
    std::vector<precondition_ast_node> conditions;
};

struct capability {
    std::string name;
    prerequisites_conjunction pre;
    outcomes post;
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

struct assignment {
    actor_accessor lhs;
    expression_node rhs;
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
