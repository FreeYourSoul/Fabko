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

#ifndef FABKO_FABL_IR_HH
#define FABKO_FABL_IR_HH

#include <nlohmann/json_fwd.hpp>
#include <variant>

#include "agent/protocol/fap_request.hh"

namespace fabko::compiler::fabl::ir {

enum class operator_exec {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
};

enum class constraint_operation {
    EQUAL,
    DIFFERENT,
    GREATER_THAN,
    LESS_THAN,
    GREATER_THAN_OR_EQUAL,
    LESS_THAN_OR_EQUAL,
};

struct source_loc {
    std::size_t line;
    std::string source_name;
};

struct actor_id {
    std::string name_;
};

struct resource {
    std::string name;

    source_loc loc;
};

struct expression {};

struct state_var {
    actor_id actor;
    resource rsc;
};

struct constraint {
    expression lhs;
    constraint_operation operation;
    expression rhs;

    source_loc loc;
};

struct effect {
    state_var lhs;
    operator_exec op;
    state_var rhs;

    source_loc loc;
};

struct capability {
    std::string name;                      //!< name of the capability
    actor_id actor;                        //!< actor offering the capability
    actor_id target;                       //!< target of the capability

    std::vector<constraint> preconditions; //!< preconditions of the capability
    std::vector<effect> effects;           //!< effect after execution of the capability

    source_loc loc;
};

enum opti_type {
    MINIMIZE,
    MAXIMIZE,
};

struct optimization {
    opti_type type;
    resource rsc;
};

struct request {
    actor_id actor;
    std::vector<resource> rsc;
    optimization opti;

    source_loc loc;
};

struct program {
    std::vector<actor_id> actors;
    std::vector<resource> resources;
    std::vector<capability> capabilities;
    request req;
};

} // namespace fabko::compiler::fabl::ir

#endif // FABKO_FABL_IR_HH
