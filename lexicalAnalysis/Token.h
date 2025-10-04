#pragma once
#include <string>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    HEX_LITERAL,
    STRING_LITERAL,
    CHAR_LITERAL,
    PREPROCESSOR_DIRECTIVE,
    COMMENT,
    OPERATOR,
    PUNCTUATOR,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    Token(TokenType t, std::string v) : type(t), value(std::move(v)) {}
};

inline std::string tokenTypeName(TokenType t) {
    switch (t) {
    case TokenType::KEYWORD: return "KEYWORD";
    case TokenType::IDENTIFIER: return "IDENTIFIER";
    case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
    case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
    case TokenType::HEX_LITERAL: return "HEX_LITERAL";
    case TokenType::STRING_LITERAL: return "STRING_LITERAL";
    case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
    case TokenType::PREPROCESSOR_DIRECTIVE: return "PREPROCESSOR_DIRECTIVE";
    case TokenType::COMMENT: return "COMMENT";
    case TokenType::OPERATOR: return "OPERATOR";
    case TokenType::PUNCTUATOR: return "PUNCTUATOR";
    default: return "UNKNOWN";
    }
}