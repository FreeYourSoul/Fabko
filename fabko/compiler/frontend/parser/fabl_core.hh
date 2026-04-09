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
        return fil::copa::match_string<fil::fixed_string {"can"}> {} +                             //
               fil::copa::match_parser<resource_grammar, fil::copa::member<&ast_object::rsc>> {} + //
               fil::copa::semicol;
    }
    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct accessor_grammar {
    using ast_object = ast::actor_accessor;

    static constexpr auto rules() {
        return (fil::copa::match_string<fil::fixed_string {"this"}, fil::copa::member<&ast_object::actor>> {}         //
                   | fil::copa::match_string<fil::fixed_string {"target"}, fil::copa::member<&ast_object::actor>> {}) //
             + fil::copa::match_char<'.'> {}                                                                          //
             + fil::copa::match_identifier<fil::copa::member<&ast_object::value>> {};                                 //
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct constraint_operation {
    using ast_object = ast::constraint_op;

    static constexpr auto rules() {                                                                            //
        return fil::copa::match_string<fil::fixed_string {"=="}, fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_string<fil::fixed_string {"!="}, fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_string<fil::fixed_string {">="}, fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_string<fil::fixed_string {">"}, fil::copa::member<&ast_object::str_to_op>> {}  //
             | fil::copa::match_string<fil::fixed_string {"<="}, fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_string<fil::fixed_string {"<"}, fil::copa::member<&ast_object::str_to_op>> {};
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct precondition_grammar {
    using ast_object = ast::precondition;

    static constexpr auto rules() {
        using target =                                                                                       //
            decltype(fil::copa::match_parser<accessor_grammar, fil::copa::member<&ast_object::lhs>> {}       //
                     + fil::copa::match_parser<constraint_operation, fil::copa::member<&ast_object::ope>> {} //
                     + fil::copa::match_identifier<fil::copa::member<&ast_object::rhs>> {}                   //
                     + fil::copa::semicol);

        using o =                                                                                            //
            decltype(fil::copa::match_identifier<fil::copa::member<&ast_object::lhs>> {}                     //
                     + fil::copa::match_parser<constraint_operation, fil::copa::member<&ast_object::ope>> {} //
                     + fil::copa::match_identifier<fil::copa::member<&ast_object::rhs>> {}                   //
                     + fil::copa::semicol);

        return target {} | o {};
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct effect_operation {
    using ast_object = ast::effect_op;

    static constexpr auto rules() {                                                     //
        return fil::copa::match_char<'+', fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_char<'-', fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_char<'*', fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_char<'/', fil::copa::member<&ast_object::str_to_op>> {} //
             | fil::copa::match_char<'%', fil::copa::member<&ast_object::str_to_op>> {};
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct effect_grammar {
    using ast_object = ast::effect;

    static constexpr auto rules() {                                                              //
        return fil::copa::match_parser<accessor_grammar, fil::copa::member<&ast_object::lhs>> {} //
             + fil::copa::match_parser<effect_operation, fil::copa::member<&ast_object::ope>> {} //
             + fil::copa::match_identifier<fil::copa::member<&ast_object::rhs>> {}               //
             + fil::copa::semicol;
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

struct capability_grammar {
    using ast_object = ast::capability;

    static constexpr auto rules() {                                                                                                 //
        return fil::copa::match_string<fil::fixed_string {"capability"}> {}                                                         //
             + fil::copa::match_char<'{'> {}                                                                                        //
             + fil::copa::match_string<fil::fixed_string {"pre"}> {}
             + fil::copa::bracket_wrapped<                                                                                          //
                 fil::copa::list_rule<fil::copa::match_parser<precondition_grammar, fil::copa::member<&ast_object::preconditions>>> //
                 > {}                                                                                                               //
             + fil::copa::match_string<fil::fixed_string {"post"}> {}
             + fil::copa::bracket_wrapped<                                                                                          //
                 fil::copa::list_rule<fil::copa::match_parser<effect_grammar, fil::copa::member<&ast_object::preconditions>>>       //
                 > {}
             + fil::copa::match_char<'}'> {};
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

    static constexpr auto rules() {                                                                           //
        return fil::copa::list_rule<                                                                          //
            fil::copa::or_rule<                                                                               //
                fil::copa::match_parser<actor_grammar, fil::copa::member<&ast_object::actors>>,               //
                fil::copa::match_parser<forced_resources_grammar, fil::copa::member<&ast_object::resources>>, //
                fil::copa::match_parser<capability_grammar, fil::copa::member<&ast_object::capabilities>>     //
                >> {};
    }

    static constexpr auto convertor() { return fil::copa::sink::aggregator<ast_object> {}; }
};

} // namespace fabko::compiler::fabl::grammar

#endif // FABKO_FABL_CORE_HH
