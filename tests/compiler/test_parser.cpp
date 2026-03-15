#include "compiler/parser/lexer.hh"
#include "compiler/parser/parser.hh"
#include <catch2/catch_test_macros.hpp>

using namespace fabko::compiler::parser;

TEST_CASE("Lexer tests", "[compiler][lexer]") {
    SECTION("Simple features") {
        std::string_view source = "feature compute; feature storage;";
        lexer l(source);
        auto tokens = l.scan_tokens();

        REQUIRE(tokens.size() == 7); // FEATURE, IDENT, SEMI, FEATURE, IDENT, SEMI, EOF
        CHECK(tokens[0].type == token_type::FEATURE);
        CHECK(tokens[1].lexeme == "compute");
        CHECK(tokens[3].type == token_type::FEATURE);
        CHECK(tokens[4].lexeme == "storage");
    }

    SECTION("Logical operators") {
        std::string_view source = "compute and storage or not gpu";
        lexer l(source);
        auto tokens = l.scan_tokens();

        CHECK(tokens[0].type == token_type::IDENTIFIER);
        CHECK(tokens[1].type == token_type::AND);
        CHECK(tokens[2].type == token_type::IDENTIFIER);
        CHECK(tokens[3].type == token_type::OR);
        CHECK(tokens[4].type == token_type::NOT);
        CHECK(tokens[5].type == token_type::IDENTIFIER);
    }
}

TEST_CASE("Parser tests", "[compiler][parser]") {
    SECTION("Feature declaration") {
        std::string_view source = "feature compute;";
        lexer l(source);
        parser p(l.scan_tokens());
        auto prog = p.parse();

        REQUIRE(prog.statements.size() == 1);
        CHECK(std::holds_alternative<feature_stmt>(prog.statements[0].data));
        CHECK(std::get<feature_stmt>(prog.statements[0].data).name.lexeme == "compute");
    }

    SECTION("Capability declaration") {
        std::string_view source = "capability worker { compute and storage }";
        lexer l(source);
        parser p(l.scan_tokens());
        auto prog = p.parse();

        REQUIRE(prog.statements.size() == 1);
        CHECK(std::holds_alternative<capability_stmt>(prog.statements[0].data));
        auto& cap = std::get<capability_stmt>(prog.statements[0].data);
        CHECK(cap.name.lexeme == "worker");
        REQUIRE(cap.expression != nullptr);
    }

    SECTION("Actor declaration") {
        std::string_view source = "actor NodeA { provides worker; constraint gpu; }";
        lexer l(source);
        parser p(l.scan_tokens());
        auto prog = p.parse();

        REQUIRE(prog.statements.size() == 1);
        CHECK(std::holds_alternative<actor_stmt>(prog.statements[0].data));
        auto& actor = std::get<actor_stmt>(prog.statements[0].data);
        CHECK(actor.name.lexeme == "NodeA");
        REQUIRE(actor.body.size() == 2);
    }
}
