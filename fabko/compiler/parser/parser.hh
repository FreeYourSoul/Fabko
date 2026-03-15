// Dual Licensing Either:
// - AGPL
// or
// - Subscription license for commercial usage (without requirement of licensing propagation).
//   please contact ballandfys@protonmail.com for additional information about this subscription commercial licensing.
//
// Created by FyS on 12.08.25. License 2022-2025
//
// In the case no license has been purchased for the use (modification or distribution in any way) of the software stack
// the APGL license is applying.
//

#ifndef FABKO_PARSER_HH
#define FABKO_PARSER_HH

#include "ast.hh"
#include "token.hh"
#include <filesystem>
#include <memory>
#include <vector>

namespace fabko::compiler::parser {

class parser {
  public:
    explicit parser(std::vector<token> tokens);
    program parse();

  private:
    stmt_node declaration();
    stmt_node feature_declaration();
    stmt_node capability_declaration();
    stmt_node actor_declaration();
    stmt_node constraint_declaration();

    expr_ptr expression();
    expr_ptr logical_or();
    expr_ptr logical_and();
    expr_ptr unary();
    expr_ptr primary();

    bool match(const std::vector<token_type>& types);
    bool check(token_type type) const;
    token advance();
    bool is_at_end() const;
    token peek() const;
    token previous() const;
    token consume(token_type type, const std::string& message);

    std::vector<token> tokens_;
    int current_ = 0;
};

} // namespace fabko::compiler::parser
#endif // FABKO_PARSER_HH
