#ifndef FABKO_COMPILER_PARSER_AST_HH
#define FABKO_COMPILER_PARSER_AST_HH

#include "token.hh"
#include <memory>
#include <variant>
#include <vector>

namespace fabko::compiler::parser {

struct expr_node;
using expr_ptr = std::unique_ptr<expr_node>;

struct binary_expr {
    expr_ptr left;
    token op;
    expr_ptr right;
};

struct unary_expr {
    token op;
    expr_ptr right;
};

struct literal_expr {
    token value;
};

struct identifier_expr {
    token name;
};

struct expr_node {
    std::variant<binary_expr, unary_expr, literal_expr, identifier_expr> data;
    explicit expr_node(auto&& d)
        : data(std::forward<decltype(d)>(d)) {}
};

struct feature_stmt {
    token name;
};

struct capability_stmt {
    token name;
    expr_ptr expression;
};

struct actor_stmt {
    token name;
    struct body_item {
        std::variant<token, expr_ptr> data; // token for 'provides', expr for constraint
    };
    std::vector<body_item> body;
};

struct constraint_stmt {
    expr_ptr expression;
};

struct stmt_node {
    std::variant<feature_stmt, capability_stmt, actor_stmt, constraint_stmt> data;
    explicit stmt_node(auto&& d)
        : data(std::forward<decltype(d)>(d)) {}
};

struct program {
    std::vector<stmt_node> statements;
};

} // namespace fabko::compiler::parser

#endif // FABKO_COMPILER_PARSER_AST_HH
