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

#include "compiler/frontend/parser/fabl_grammar.hh"

TEST_CASE("fabl parsing", "[compiler][frontend]") {

    SECTION("parsing accessor : empty") {
        std::string content = R"(
           actor dada { };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->name == "dada");
        CHECK(v->content.size() == 0);
    }
    SECTION("parsing accessor : 1 has") {
        std::string content = R"(
           actor dada {
                has 4 chocobo;
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->name == "dada");
        REQUIRE(v->content.size() == 1);
        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::has_statement>(v->content[0]));

        const auto& has1 = std::get<fabko::compiler::fabl::cst::has_statement>(v->content[0]);

        CHECK(has1.id == "chocobo");
        CHECK(has1.quantity == 4);
    }

    SECTION("parsing accessor : multiple has") {
        std::string content = R"(
           actor ff7 {
                has 4 chocobo;
                has 2 cloud;
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->name == "ff7");
        REQUIRE(v->content.size() == 2);
        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::has_statement>(v->content[0]));

        const auto& has1 = std::get<fabko::compiler::fabl::cst::has_statement>(v->content[0]);
        CHECK(has1.id == "chocobo");
        CHECK(has1.quantity == 4);

        const auto& has2 = std::get<fabko::compiler::fabl::cst::has_statement>(v->content[1]);
        CHECK(has2.id == "cloud");
        CHECK(has2.quantity == 2);
    }

    SECTION("parsing accessor : can single identifier") {
        std::string content = R"(
           actor chocobo {
                can fly;
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 1);
        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability_identifier>(v->content[0]));

        const auto& cap = std::get<fabko::compiler::fabl::cst::capability_identifier>(v->content[0]);
        CHECK(cap.id == "fly");
    }
}
