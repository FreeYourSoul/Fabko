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

#ifndef FABKO_FABL_CORE_HH
#define FABKO_FABL_CORE_HH

#include "fil/copa/copa.hh"
#include "fil/copa/matcher.hh"

#include "ast.hh"

namespace fabko::compiler::fabl::grammar {

struct actor_grammar {
    using ast_object = ast::actor;

    static constexpr auto rules() { //
        return fil::copa::eof;
    }

    static constexpr auto producer() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct fabl {
    using ast_object = ast::fabl;

    static constexpr auto rules() { //
        return fil::copa::match_parser<actor_grammar> {};
    }

    static constexpr auto producer() { return fil::copa::sink::aggregator<ast_object> {}; }
};

} // namespace fabko::compiler::fabl::grammar

#endif // FABKO_FABL_CORE_HH
