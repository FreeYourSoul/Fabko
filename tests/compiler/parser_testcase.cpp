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

// printer specialization
namespace fil {
template<> std::string to_string(const fabko::compiler::fabl::cst::compare_operator& op) { return std::to_string(static_cast<std::uint32_t>(op)); }
template<> std::string to_string(const fabko::compiler::fabl::cst::actor_accessor& aa) { return std::format("[{} {}]", aa.actor, aa.access.value_or("none")); }
} // namespace fil

TEST_CASE("fabl parsing", "[compiler][frontend]") {

    SECTION("parsing actor : empty") {
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
    SECTION("parsing actor : 1 has") {
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

    SECTION("parsing actor : multiple has") {
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

    SECTION("parsing actor : can single identifier") {
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

    SECTION("parsing actor : can multiple identifiers") {
        std::string content = R"(
           actor chocobo {
                can fly;
                can sleep;
                can scream;
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 3);

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability_identifier>(v->content[0]));
        const auto& cap1 = std::get<fabko::compiler::fabl::cst::capability_identifier>(v->content[0]);
        CHECK(cap1.id == "fly");

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability_identifier>(v->content[1]));
        const auto& cap2 = std::get<fabko::compiler::fabl::cst::capability_identifier>(v->content[1]);
        CHECK(cap2.id == "sleep");

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability_identifier>(v->content[2]));
        const auto& cap3 = std::get<fabko::compiler::fabl::cst::capability_identifier>(v->content[2]);
        CHECK(cap3.id == "scream");
    }

    SECTION("parsing actor : can full single capability") {
        std::string content = R"(
           actor chocobo {
                can capability fly {
                    pre { }
                    post { }
                };
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 1);
        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability>(v->content[0]));

        const auto& cap = std::get<fabko::compiler::fabl::cst::capability>(v->content[0]);
        CHECK(cap.name == "fly");
    }

    SECTION("parsing actor : can full multiple capabilities") {
        std::string content = R"(
           actor chocobo {
                can capability fly {
                    pre { }
                    post { }
                };
                can capability sleep {
                    pre { }
                    post { }
                };
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());
        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 2);

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability>(v->content[0]));
        const auto& cap = std::get<fabko::compiler::fabl::cst::capability>(v->content[0]);
        CHECK(cap.name == "fly");

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability>(v->content[1]));
        const auto& cap2 = std::get<fabko::compiler::fabl::cst::capability>(v->content[1]);
        CHECK(cap2.name == "sleep");
    }

    SECTION("parsing actor : pre with single condition") {
        std::string content = R"(
           actor chocobo {
                can capability fly {
                    pre { self.altitude == 0; }
                    post { }
                };
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());

        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 1);

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability>(v->content[0]));
        const auto& cap = std::get<fabko::compiler::fabl::cst::capability>(v->content[0]);
        CHECK(cap.name == "fly");

        REQUIRE(cap.pre.conditions.size() == 1);
        const auto& precondition1 = cap.pre.conditions[0];
        std::println("{}", fil::to_string(precondition1));

        CHECK(precondition1.value == fabko::compiler::fabl::cst::compare_operator::EQUAL);

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::actor_accessor>(precondition1.lhs));
        const auto& lhs = std::get<fabko::compiler::fabl::cst::actor_accessor>(precondition1.lhs);

        CHECK(lhs.actor == "self");
        CHECK(lhs.access == "altitude");

        REQUIRE(std::holds_alternative<int>(precondition1.rhs));
        const auto rhs = std::get<int>(precondition1.rhs);

        CHECK(rhs == 0);
    }

    SECTION("parsing actor : pre with multiple condition") {
        std::string content = R"(
           actor chocobo {
                can capability fly {
                    pre {
                        self.altitude == 0;
                        self.will >= 42;
                        self.strength > 1337;
                    }
                    post { }
                };
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());

        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 1);

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability>(v->content[0]));
        const auto& cap = std::get<fabko::compiler::fabl::cst::capability>(v->content[0]);
        CHECK(cap.name == "fly");

        REQUIRE(cap.pre.conditions.size() == 3);

        SECTION("test precondition index 0") {
            const auto& precondition = cap.pre.conditions[0];
            std::println("---\n precondition index 0 : \n {}", fil::to_string(precondition));

            CHECK(precondition.value == fabko::compiler::fabl::cst::compare_operator::EQUAL);

            REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::actor_accessor>(precondition.lhs));
            const auto& lhs = std::get<fabko::compiler::fabl::cst::actor_accessor>(precondition.lhs);

            CHECK(lhs.actor == "self");
            CHECK(lhs.access == "altitude");

            REQUIRE(std::holds_alternative<int>(precondition.rhs));
            const auto rhs = std::get<int>(precondition.rhs);

            CHECK(rhs == 0);
        }

        SECTION("test precondition index 1") {
            const auto& precondition = cap.pre.conditions[1];
            std::println("---\n precondition index 1 : \n {}", fil::to_string(precondition));

            CHECK(precondition.value == fabko::compiler::fabl::cst::compare_operator::GREATER_EQUAL);

            REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::actor_accessor>(precondition.lhs));
            const auto& lhs = std::get<fabko::compiler::fabl::cst::actor_accessor>(precondition.lhs);

            CHECK(lhs.actor == "self");
            CHECK(lhs.access == "will");

            REQUIRE(std::holds_alternative<int>(precondition.rhs));
            const auto rhs = std::get<int>(precondition.rhs);

            CHECK(rhs == 42);
        }

        SECTION("test precondition index 2") {
            const auto& precondition = cap.pre.conditions[2];
            std::println("---\n precondition index 2 : \n {}", fil::to_string(precondition));

            CHECK(precondition.value == fabko::compiler::fabl::cst::compare_operator::GREATER);

            REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::actor_accessor>(precondition.lhs));
            const auto& lhs = std::get<fabko::compiler::fabl::cst::actor_accessor>(precondition.lhs);

            CHECK(lhs.actor == "self");
            CHECK(lhs.access == "strength");

            REQUIRE(std::holds_alternative<int>(precondition.rhs));
            const auto rhs = std::get<int>(precondition.rhs);

            CHECK(rhs == 1337);
        }
    }

    SECTION("test postcondition single assignment") {
        std::string content = R"(
           actor chocobo {
                can capability fly {
                    pre {}
                    post {
                        self.altitude = 42;
                    }
                };
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());

        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 1);

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability>(v->content[0]));
        const auto& cap = std::get<fabko::compiler::fabl::cst::capability>(v->content[0]);
        CHECK(cap.name == "fly");

        REQUIRE(cap.pre.conditions.empty());
        REQUIRE(cap.post.assignments.size() == 1);

        const auto& assignment = cap.post.assignments[0];
        CHECK(assignment.lhs.actor == "self");
        CHECK(assignment.lhs.access == "altitude");

        CHECK(std::holds_alternative<std::monostate>(assignment.rhs.rhs));
        CHECK(std::holds_alternative<fabko::compiler::fabl::cst::literal_integer>(assignment.rhs.lhs));
        CHECK(std::get<fabko::compiler::fabl::cst::literal_integer>(assignment.rhs.lhs) == 42);
    }

    SECTION("test postcondition multiple assignments") {
        std::string content = R"(
           actor chocobo {
                can capability fly {
                    pre {}
                    post {
                        self.altitude = 42;
                        self.notoriety = true;
                    }
                };
            };
        )";

        fil::buffer_reader reader(std::move(content));

        auto g       = fabko::compiler::fabl::grammar::actor_definition {};
        const auto v = fil::copa::parse(g, std::move(reader));

        REQUIRE(v.has_value());

        CHECK(v->name == "chocobo");
        REQUIRE(v->content.size() == 1);

        REQUIRE(std::holds_alternative<fabko::compiler::fabl::cst::capability>(v->content[0]));
        const auto& cap = std::get<fabko::compiler::fabl::cst::capability>(v->content[0]);
        CHECK(cap.name == "fly");

        REQUIRE(cap.pre.conditions.empty());
        REQUIRE(cap.post.assignments.size() == 2);

        SECTION("assignment #1") {
            const auto& assignment = cap.post.assignments[0];

            CHECK(assignment.lhs.actor == "self");
            CHECK(assignment.lhs.access == "altitude");

            CHECK(std::holds_alternative<std::monostate>(assignment.rhs.rhs));
            CHECK(std::holds_alternative<fabko::compiler::fabl::cst::literal_integer>(assignment.rhs.lhs));
            CHECK(std::get<fabko::compiler::fabl::cst::literal_integer>(assignment.rhs.lhs) == 42);
        }

        SECTION("assignment #2") {
            const auto& assignment = cap.post.assignments[1];

            CHECK(assignment.lhs.actor == "self");
            CHECK(assignment.lhs.access == "notoriety");

            CHECK(std::holds_alternative<std::monostate>(assignment.rhs.rhs));
            CHECK(std::holds_alternative<fabko::compiler::fabl::cst::literal_bool>(assignment.rhs.lhs));
            CHECK(std::get<fabko::compiler::fabl::cst::literal_bool>(assignment.rhs.lhs).value);
        }
    }
}
