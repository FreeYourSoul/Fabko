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
    std::vector<resource> resources;
};

struct actor {
    std::string name;

    resources_hardcoded resources;
};

struct fabl_program {};

} // namespace fabko::compiler::fabl::ast

#endif // FABKO_AST_HH
