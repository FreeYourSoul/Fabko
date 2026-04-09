// Dual Licensing Either :
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 18.07.25. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <fil/copa/copa.hh>
#include <fil/meta/buffer_reader.hh>

#include "compiler/frontend/parser/fabl_core.hh"

TEST_CASE("fabl parsing", "[compiler][frontend]") {

    SECTION("parsing accessor : target") {
        std::string content = R"(
           target.chocobo
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::accessor_grammar {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->actor == "target");
        CHECK(v->value == "chocobo");
    }

    SECTION("parsing accessor : this") {
        std::string content = R"(
           this.chocobo
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::accessor_grammar {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->actor == "this");
        CHECK(v->value == "chocobo");
    }

    SECTION("parsing precondition") {
        auto g = fabko::compiler::fabl::grammar::precondition_grammar {};

        auto operation = GENERATE(                                                                                  //
            std::pair {std::string {"=="}, fabko::compiler::fabl::ir::constraint_operation::EQUAL},                 //
            std::pair {std::string {"!="}, fabko::compiler::fabl::ir::constraint_operation::DIFFERENT},             //
            std::pair {std::string {">"}, fabko::compiler::fabl::ir::constraint_operation::GREATER_THAN},           //
            std::pair {std::string {">="}, fabko::compiler::fabl::ir::constraint_operation::GREATER_THAN_OR_EQUAL}, //
            std::pair {std::string {"<"}, fabko::compiler::fabl::ir::constraint_operation::LESS_THAN},
            std::pair {std::string {"<="}, fabko::compiler::fabl::ir::constraint_operation::LESS_THAN_OR_EQUAL});

        SECTION("lhs : accessor") {
            auto actor_selected = GENERATE(std::string {"target"}, std::string {"this"});
            auto element        = GENERATE(std::string {"chocobo"}, std::string {"ifrit"}, std::string {"luna"});
            auto rhs            = GENERATE(std::string {"1"}, std::string {"42"}, std::string {"1337"});

            std::string content = std::format(R"(
               {}.{} {} {};
            )",
                actor_selected,
                element,
                operation.first,
                rhs);

            INFO("Testing with value: operation: " << operation.first << ", actor_selected:" << actor_selected << ", element:" << element << ", rhs:" << rhs);
            INFO("string parsed: " << content);

            fil::buffer_reader reader(std::move(content));

            const auto v = fil::copa::parse(g, std::move(reader));

            REQUIRE(v.has_value());

            CHECK(std::holds_alternative<fabko::compiler::fabl::ast::actor_accessor>(v->lhs));
            CHECK(std::get<fabko::compiler::fabl::ast::actor_accessor>(v->lhs).actor == actor_selected);
            CHECK(std::get<fabko::compiler::fabl::ast::actor_accessor>(v->lhs).value == element);

            CHECK(v->ope.op == operation.second);

            CHECK(std::holds_alternative<std::string>(v->rhs));
            CHECK(std::get<std::string>(v->rhs) == rhs);
        }

        SECTION("lhs : identifier") {
            auto actor_selected = GENERATE(std::string {"chocobo"}, std::string {"yuna"});
            auto rhs            = GENERATE(std::string {"1"}, std::string {"42"}, std::string {"1337"});

            std::string content = std::format(R"(
               {} {} {};
            )",
                actor_selected,
                operation.first,
                rhs);

            INFO("Testing with value: operation: " << operation.first << ", actor_selected:" << actor_selected << ", rhs:" << rhs);
            INFO("string parsed: " << content);

            fil::buffer_reader reader(std::move(content));

            const auto v = fil::copa::parse(g, std::move(reader));

            REQUIRE(v.has_value());

            CHECK(std::holds_alternative<std::string>(v->lhs));
            CHECK(std::get<std::string>(v->lhs) == actor_selected);
            CHECK(v->ope.op == operation.second);
            CHECK(std::holds_alternative<std::string>(v->rhs));
            CHECK(std::get<std::string>(v->rhs) == rhs);
        }
    }

    SECTION("parsing effect") {
        auto g = fabko::compiler::fabl::grammar::effect_grammar {};

        auto actor_selected = GENERATE(std::string {"target"}, std::string {"this"});
        auto element        = GENERATE(std::string {"chocobo"}, std::string {"ifrit"}, std::string {"luna"});
        auto value          = GENERATE(std::string {"1"}, std::string {"42"}, std::string {"1337"});
        auto operation      = GENERATE(                                                             //
            std::pair {std::string {"+"}, fabko::compiler::fabl::ir::operator_exec::ADD},      //
            std::pair {std::string {"-"}, fabko::compiler::fabl::ir::operator_exec::SUBTRACT}, //
            std::pair {std::string {"*"}, fabko::compiler::fabl::ir::operator_exec::MULTIPLY}, //
            std::pair {std::string {"/"}, fabko::compiler::fabl::ir::operator_exec::DIVIDE},   //
            std::pair {std::string {"%"}, fabko::compiler::fabl::ir::operator_exec::MODULO});

        std::string content = std::format(R"(
               {}.{} {} {};
            )",
            actor_selected,
            element,
            operation.first,
            value);

        fil::buffer_reader reader(std::move(content));

        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->lhs.actor == actor_selected);
        CHECK(v->lhs.value == element);
        CHECK(v->ope.op == operation.second);
        CHECK(v->rhs == value);
    }
}
