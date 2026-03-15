#ifndef FABKO_COMPILER_PARSER_TOKEN_HH
#define FABKO_COMPILER_PARSER_TOKEN_HH

#include <string>
#include <string_view>

namespace fabko::compiler::parser {

enum class token_type {
    // Keywords
    FEATURE,
    CAPABILITY,
    ACTOR,
    PROVIDES,
    CONSTRAINT,
    ALIAS,
    STRUCT,

    // Logical operators
    AND,
    OR,
    NOT,
    TRUE,
    FALSE,

    // Punctuation
    LEFT_BRACE,   // {
    RIGHT_BRACE,  // }
    LEFT_PAREN,   // (
    RIGHT_PAREN,  // )
    LEFT_SQUARE,  // [
    RIGHT_SQUARE, // ]
    SEMICOLON,    // ;
    COLON,        // :
    DOUBLE_COLON, // ::
    EQUALS,       // =
    COMMA,        // ,
    DOT,          // .

    // Literals
    IDENTIFIER,
    STRING,
    NUMBER,

    // Special
    END_OF_FILE,
    UNKNOWN
};

struct token {
    token_type type;
    std::string lexeme;
    int line;
    int column;

    token(token_type t, std::string l, int ln, int col)
        : type(t)
        , lexeme(std::move(l))
        , line(ln)
        , column(col) {}
};

std::string_view to_string(token_type type);

} // namespace fabko::compiler::parser

#endif // FABKO_COMPILER_PARSER_TOKEN_HH
