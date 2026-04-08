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
#include <fil/copa/ast.hh>

#include "ast.hh"

namespace fabko::compiler::fabl::grammar {

struct resource_grammar {
    using ast_object = ast::resource;

    static constexpr auto rules() { return fil::copa::match_identifier<fil::copa::member<&ast_object::name>> {} + fil::copa::semicol; }
    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct forced_resources_grammar {
    using ast_object = ast::resources_hardcoded;

    static constexpr auto rules() {
        return fil::copa::match_string<fil::fixed_string {"can"}> {}                                  //
             + fil::copa::bracket_wrapped<                                                            //
                 fil::copa::match_parser<resource_grammar, fil::copa::member<&ast_object::resources>> //
                 > {};
    }
    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct actor_grammar {
    using ast_object = ast::actor;

    static constexpr auto rules() { //
        return fil::copa::eof;
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct fabl {
    using ast_object = ast::fabl_program;

    static constexpr auto rules() { //
        return fil::copa::match_parser<actor_grammar> {};
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

static_assert(fil::copa::production<resource_grammar>);
static_assert(fil::copa::production<forced_resources_grammar>);
static_assert(fil::copa::production<forced_resources_grammar>);
static_assert(fil::copa::production<actor_grammar>);
static_assert(fil::copa::production<fabl>);

} // namespace fabko::compiler::fabl::grammar

#endif // FABKO_FABL_CORE_HH
