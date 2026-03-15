#ifndef FABKO_COMPILER_PARSER_LEXER_HH
#define FABKO_COMPILER_PARSER_LEXER_HH

#include "token.hh"
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace fabko::compiler::parser {

class lexer {
  public:
    explicit lexer(std::string_view source);

    std::vector<token> scan_tokens();

  private:
    void scan_token();
    void add_token(token_type type);
    void add_token(token_type type, std::string lexeme);
    bool is_at_end() const;
    char advance();
    char peek() const;
    char peek_next() const;
    bool match(char expected);
    void identifier();
    void number();
    void string();

    std::string_view source_;
    std::vector<token> tokens_;
    int start_   = 0;
    int current_ = 0;
    int line_    = 1;
    int column_  = 0;

    static const std::map<std::string, token_type> keywords;
};

} // namespace fabko::compiler::parser

#endif // FABKO_COMPILER_PARSER_LEXER_HH
