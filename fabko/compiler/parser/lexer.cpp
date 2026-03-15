#include "lexer.hh"
#include <cctype>

namespace fabko::compiler::parser {

const std::map<std::string, token_type> lexer::keywords = {
    {"feature",    token_type::FEATURE   },
    {"capability", token_type::CAPABILITY},
    {"actor",      token_type::ACTOR     },
    {"provides",   token_type::PROVIDES  },
    {"constraint", token_type::CONSTRAINT},
    {"alias",      token_type::ALIAS     },
    {"struct",     token_type::STRUCT    },
    {"and",        token_type::AND       },
    {"or",         token_type::OR        },
    {"not",        token_type::NOT       },
    {"true",       token_type::TRUE      },
    {"false",      token_type::FALSE     }
};

std::string_view to_string(token_type type) {
    switch (type) {
        case token_type::FEATURE: return "FEATURE";
        case token_type::CAPABILITY: return "CAPABILITY";
        case token_type::ACTOR: return "ACTOR";
        case token_type::PROVIDES: return "PROVIDES";
        case token_type::CONSTRAINT: return "CONSTRAINT";
        case token_type::ALIAS: return "ALIAS";
        case token_type::STRUCT: return "STRUCT";
        case token_type::AND: return "AND";
        case token_type::OR: return "OR";
        case token_type::NOT: return "NOT";
        case token_type::TRUE: return "TRUE";
        case token_type::FALSE: return "FALSE";
        case token_type::LEFT_BRACE: return "LEFT_BRACE";
        case token_type::RIGHT_BRACE: return "RIGHT_BRACE";
        case token_type::LEFT_PAREN: return "LEFT_PAREN";
        case token_type::RIGHT_PAREN: return "RIGHT_PAREN";
        case token_type::LEFT_SQUARE: return "LEFT_SQUARE";
        case token_type::RIGHT_SQUARE: return "RIGHT_SQUARE";
        case token_type::SEMICOLON: return "SEMICOLON";
        case token_type::COLON: return "COLON";
        case token_type::DOUBLE_COLON: return "DOUBLE_COLON";
        case token_type::EQUALS: return "EQUALS";
        case token_type::COMMA: return "COMMA";
        case token_type::DOT: return "DOT";
        case token_type::IDENTIFIER: return "IDENTIFIER";
        case token_type::STRING: return "STRING";
        case token_type::NUMBER: return "NUMBER";
        case token_type::END_OF_FILE: return "END_OF_FILE";
        case token_type::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

lexer::lexer(std::string_view source)
    : source_(source) {}

std::vector<token> lexer::scan_tokens() {
    while (!is_at_end()) {
        start_ = current_;
        scan_token();
    }
    tokens_.emplace_back(token_type::END_OF_FILE, "", line_, column_);
    return tokens_;
}

void lexer::scan_token() {
    char c = advance();
    switch (c) {
        case '{': add_token(token_type::LEFT_BRACE); break;
        case '}': add_token(token_type::RIGHT_BRACE); break;
        case '(': add_token(token_type::LEFT_PAREN); break;
        case ')': add_token(token_type::RIGHT_PAREN); break;
        case '[': add_token(token_type::LEFT_SQUARE); break;
        case ']': add_token(token_type::RIGHT_SQUARE); break;
        case ';': add_token(token_type::SEMICOLON); break;
        case ',': add_token(token_type::COMMA); break;
        case '.': add_token(token_type::DOT); break;
        case '=': add_token(token_type::EQUALS); break;
        case ':':
            if (match(':'))
                add_token(token_type::DOUBLE_COLON);
            else
                add_token(token_type::COLON);
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            line_++;
            column_ = 0;
            break;
        case '"': string(); break;
        case '/':
            if (match('/')) {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !is_at_end())
                    advance();
            } else {
                add_token(token_type::UNKNOWN);
            }
            break;
        default:
            if (std::isdigit(c)) {
                number();
            } else if (std::isalpha(c) || c == '_') {
                identifier();
            } else {
                add_token(token_type::UNKNOWN);
            }
            break;
    }
}

void lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_')
        advance();

    std::string text(source_.substr(start_, current_ - start_));
    token_type type = token_type::IDENTIFIER;
    if (auto it = keywords.find(text); it != keywords.end()) {
        type = it->second;
    }
    add_token(type, std::move(text));
}

void lexer::number() {
    while (std::isdigit(peek()))
        advance();

    // Look for a fractional part.
    if (peek() == '.' && std::isdigit(peek_next())) {
        // Consume the "."
        advance();
        while (std::isdigit(peek()))
            advance();
    }

    add_token(token_type::NUMBER, std::string(source_.substr(start_, current_ - start_)));
}

void lexer::string() {
    while (peek() != '"' && !is_at_end()) {
        if (peek() == '\n')
            line_++;
        advance();
    }

    if (is_at_end()) {
        add_token(token_type::UNKNOWN);
        return;
    }

    // The closing ".
    advance();

    // Trim the surrounding quotes.
    std::string value(source_.substr(start_ + 1, current_ - start_ - 2));
    add_token(token_type::STRING, std::move(value));
}

bool lexer::is_at_end() const { return static_cast<size_t>(current_) >= source_.length(); }

char lexer::advance() {
    column_++;
    return source_[current_++];
}

char lexer::peek() const {
    if (is_at_end())
        return '\0';
    return source_[current_];
}

char lexer::peek_next() const {
    if (static_cast<size_t>(current_ + 1) >= source_.length())
        return '\0';
    return source_[current_ + 1];
}

bool lexer::match(char expected) {
    if (is_at_end())
        return false;
    if (source_[current_] != expected)
        return false;

    current_++;
    column_++;
    return true;
}

void lexer::add_token(token_type type) { add_token(type, std::string(source_.substr(start_, current_ - start_))); }

void lexer::add_token(token_type type, std::string lexeme) { tokens_.emplace_back(type, std::move(lexeme), line_, column_); }

} // namespace fabko::compiler::parser
