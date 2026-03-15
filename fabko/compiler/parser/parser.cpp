#include "parser.hh"
#include <fmt/format.h>
#include <stdexcept>

namespace fabko::compiler::parser {

parser::parser(std::vector<token> tokens)
    : tokens_(std::move(tokens)) {}

program parser::parse() {
    program p;
    while (!is_at_end()) {
        try {
            if (match({token_type::FEATURE}))
                p.statements.push_back(feature_declaration());
            else if (match({token_type::CAPABILITY}))
                p.statements.push_back(capability_declaration());
            else if (match({token_type::ACTOR}))
                p.statements.push_back(actor_declaration());
            else if (match({token_type::CONSTRAINT}))
                p.statements.push_back(constraint_declaration());
            else
                throw std::runtime_error(fmt::format("[{}:{}] Expect declaration.", peek().line, peek().column));
        } catch (const std::runtime_error& e) {
            // Basic error recovery: skip until next semicolon or keyword
            advance();
            while (!is_at_end() && peek().type != token_type::SEMICOLON && peek().type != token_type::FEATURE && peek().type != token_type::CAPABILITY
                   && peek().type != token_type::ACTOR) {
                advance();
            }
            if (!is_at_end() && peek().type == token_type::SEMICOLON)
                advance();
        }
    }
    return p;
}

stmt_node parser::feature_declaration() {
    token name = consume(token_type::IDENTIFIER, "Expect feature name.");
    consume(token_type::SEMICOLON, "Expect ';' after feature declaration.");
    return stmt_node(feature_stmt {name});
}

stmt_node parser::capability_declaration() {
    token name = consume(token_type::IDENTIFIER, "Expect capability name.");
    consume(token_type::LEFT_BRACE, "Expect '{' before capability body.");
    expr_ptr expr = expression();
    consume(token_type::RIGHT_BRACE, "Expect '}' after capability body.");
    return stmt_node(capability_stmt {name, std::move(expr)});
}

stmt_node parser::actor_declaration() {
    token name = consume(token_type::IDENTIFIER, "Expect actor name.");
    consume(token_type::LEFT_BRACE, "Expect '{' before actor body.");

    std::vector<actor_stmt::body_item> body;
    while (!check(token_type::RIGHT_BRACE) && !is_at_end()) {
        if (match({token_type::PROVIDES})) {
            token cap = consume(token_type::IDENTIFIER, "Expect capability name after 'provides'.");
            consume(token_type::SEMICOLON, "Expect ';' after provides.");
            body.push_back({cap});
        } else if (match({token_type::CONSTRAINT})) {
            expr_ptr expr = expression();
            consume(token_type::SEMICOLON, "Expect ';' after constraint.");
            body.push_back({std::move(expr)});
        } else {
            throw std::runtime_error(fmt::format("[{}:{}] Expect 'provides' or 'constraint' in actor body.", peek().line, peek().column));
        }
    }

    consume(token_type::RIGHT_BRACE, "Expect '}' after actor body.");
    return stmt_node(actor_stmt {name, std::move(body)});
}

stmt_node parser::constraint_declaration() {
    expr_ptr expr = expression();
    consume(token_type::SEMICOLON, "Expect ';' after constraint.");
    return stmt_node(constraint_stmt {std::move(expr)});
}

expr_ptr parser::expression() { return logical_or(); }

expr_ptr parser::logical_or() {
    expr_ptr expr = logical_and();

    while (match({token_type::OR})) {
        token op       = previous();
        expr_ptr right = logical_and();
        auto new_node  = std::make_unique<expr_node>(binary_expr {std::move(expr), op, std::move(right)});
        expr           = std::move(new_node);
    }

    return expr;
}

expr_ptr parser::logical_and() {
    expr_ptr expr = unary();

    while (match({token_type::AND})) {
        token op       = previous();
        expr_ptr right = unary();
        auto new_node  = std::make_unique<expr_node>(binary_expr {std::move(expr), op, std::move(right)});
        expr           = std::move(new_node);
    }

    return expr;
}

expr_ptr parser::unary() {
    if (match({token_type::NOT})) {
        token op       = previous();
        expr_ptr right = unary();
        return std::make_unique<expr_node>(unary_expr {op, std::move(right)});
    }

    return primary();
}

expr_ptr parser::primary() {
    if (match({token_type::FALSE}))
        return std::make_unique<expr_node>(literal_expr {previous()});
    if (match({token_type::TRUE}))
        return std::make_unique<expr_node>(literal_expr {previous()});

    if (match({token_type::IDENTIFIER})) {
        return std::make_unique<expr_node>(identifier_expr {previous()});
    }

    if (match({token_type::LEFT_PAREN})) {
        expr_ptr expr = expression();
        consume(token_type::RIGHT_PAREN, "Expect ')' after expression.");
        return expr;
    }

    throw std::runtime_error(fmt::format("[{}:{}] Expect expression.", peek().line, peek().column));
}

bool parser::match(const std::vector<token_type>& types) {
    for (token_type type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool parser::check(token_type type) const {
    if (is_at_end())
        return false;
    return peek().type == type;
}

token parser::advance() {
    if (!is_at_end())
        current_++;
    return previous();
}

bool parser::is_at_end() const { return peek().type == token_type::END_OF_FILE; }

token parser::peek() const { return tokens_[current_]; }

token parser::previous() const { return tokens_[current_ - 1]; }

token parser::consume(token_type type, const std::string& message) {
    if (check(type))
        return advance();
    throw std::runtime_error(fmt::format("[{}:{}] {}", peek().line, peek().column, message));
}

} // namespace fabko::compiler::parser