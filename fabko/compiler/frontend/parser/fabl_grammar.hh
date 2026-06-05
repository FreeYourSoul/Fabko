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
#include "fil/copa/wrapper_utils.hh"

#include "concrete_ast.hh"

namespace fabko::compiler::fabl::grammar {

namespace keywords {
static constexpr auto ACTOR           = fil::fixed_string {"actor"};
static constexpr auto ACTOR_CAN       = fil::fixed_string {"can"};
static constexpr auto ACTOR_HAS       = fil::fixed_string {"has"};
static constexpr auto CAPABILITY      = fil::fixed_string {"capability"};
static constexpr auto CAPABILITY_PRE  = fil::fixed_string {"pre"};
static constexpr auto CAPABILITY_POST = fil::fixed_string {"post"};

using match_actor      = fil::copa::match_string<ACTOR>;
using match_can        = fil::copa::match_string<ACTOR_CAN>;
using match_has        = fil::copa::match_string<ACTOR_HAS>;
using match_capability = fil::copa::match_string<CAPABILITY>;
using match_pre        = fil::copa::match_string<CAPABILITY_PRE>;
using match_post       = fil::copa::match_string<CAPABILITY_POST>;

static constexpr auto match_any_keyword = fil::copa::or_rule< //
    match_actor, match_can, match_capability, match_has, match_pre, match_post> {};

} // namespace keywords

struct capability_pre_statement {
    using ast_object = concrete_ast::precondition_ast_node;

    static constexpr auto rules() {
        return keywords::match_actor {} //
             + fil::copa::bracketed(fil::copa::match_identifier {});
    }

    static constexpr auto convertor() { return fil::copa::sink::ast_tree_generator<ast_object> {}; }
};
struct capability_post_statement {
    using ast_object = concrete_ast::effect_ast_node;

    static constexpr auto rules() {
        return keywords::match_post {} //
             + fil::copa::bracketed(fil::copa::match_identifier {});
    }

    static constexpr auto convertor() { return fil::copa::sink::ast_tree_generator<ast_object> {}; }
};

struct capability_definition {
    using ast_object = concrete_ast::capability;

    static constexpr auto rules() {
        return keywords::match_capability {}
             + fil::copa::bracketed(                                         //
                 fil::copa::match_production<capability_pre_statement> {}    //
                 + fil::copa::match_production<capability_post_statement> {} //
             );
    }
    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct actor_can_statement {
    struct ast_object {};

    static constexpr auto rules() {
        return keywords::match_can {}                                 //
             + (fil::copa::match_production<capability_definition> {} //
                 | (fil::copa::match_number {} + fil::copa::match_identifier {}));
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct actor_has_statement {
    using ast_object = concrete_ast::has_statement;

    static constexpr auto rules() {
        return keywords::match_has {}                                               //
             + fil::copa::match_number<fil::copa::member<&ast_object::quantity>> {} //
             + fil::copa::match_identifier<fil::copa::member<&ast_object::id>> {}   //
             + fil::copa::semicol;
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct actor_definition {
    using ast_object = concrete_ast::custom_data_type;

    static constexpr auto rules() {
        //
        return keywords::match_actor {} + fil::copa::match_identifier<fil::copa::member<&ast_object::name>> {} //
             + fil::copa::bracketed(fil::copa::list(fil::copa::or_rule<                                        //
                 fil::copa::match_parser<actor_has_statement, fil::copa::member<&ast_object::content>>,
                 fil::copa::match_parser<actor_can_statement>                                                  //
                 > {}                                                                                          //
                 ))
             + fil::copa::semicol;
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct fabl {
    struct ast_object {
        std::vector<actor_definition::ast_object> actors;
        std::vector<capability_definition::ast_object> capabilities;

        fil::copa::debug_info copa_debug_info;
    };

    static constexpr auto rules() {                                                                          //
        return fil::copa::list(                                                                              //
            fil::copa::or_rule<                                                                              //
                fil::copa::match_parser<actor_definition, fil::copa::member<&ast_object::actors>>,           //
                fil::copa::match_parser<capability_definition, fil::copa::member<&ast_object::capabilities>> //
                > {});
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

} // namespace fabko::compiler::fabl::grammar

#endif // FABKO_FABL_CORE_HH
