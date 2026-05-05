#include "Lexer.h"
#include <algorithm>
#include <stdexcept>

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    while (!eof()) {
        char c = peek();

        if (isWhitespace(c)) { handleWhitespace(); continue; }

        if (c == '#' && atLineStart) { tokens.push_back(readPreprocessor()); continue; }
        if (c == '/' && peek(1) == '/') { tokens.push_back(readComment()); continue; }
        if (c == '"') { tokens.push_back(readString()); continue; }
        if (c == '\'') { tokens.push_back(readChar()); continue; }
        if (isAlpha(c) || c == '@') { tokens.push_back(readIdentifier()); continue; }
        if (isDigit(c)) { tokens.push_back(readNumber()); continue; }

        Token op = tryReadOperator();
        if (op.type != TokenType::UNKNOWN) { tokens.push_back(op); continue; }

        Token punc = tryReadPunctuator();
        if (punc.type != TokenType::UNKNOWN) { tokens.push_back(punc); continue; }

        tokens.emplace_back(TokenType::UNKNOWN, string(1, get()));
        atLineStart = false;
    }
    return tokens;
}

void Lexer::handleWhitespace() {
    if (peek() == '\n') atLineStart = true;
    get();
}

void Lexer::initKeywords() {
    keywords = {
        "abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char", "checked",
        "class", "const", "continue", "decimal", "default", "delegate", "do", "double", "else",
        "enum", "event", "explicit", "extern", "false", "finally", "fixed", "float", "for", "foreach",
        "goto", "if", "implicit", "in", "int", "interface", "internal", "is", "lock", "long",
        "namespace", "new", "null", "object", "operator", "out", "override", "params", "private",
        "protected", "public", "readonly", "ref", "return", "sbyte", "sealed", "short", "sizeof",
        "stackalloc", "static", "string", "struct", "switch", "this", "throw", "true", "try", "typeof",
        "uint", "ulong", "unchecked", "unsafe", "ushort", "using", "virtual", "void", "volatile", "while"
    };
}

void Lexer::initOps() {
    twoCharOps = { "==", "<=", ">=", "!=", "&&", "||", "++", "--", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "->", "??", "::", "<<", ">>", "=>" };
    threeCharOps = { "<<=", ">>=" };
}

Token Lexer::readPreprocessor() {
    size_t start = pos;
    while (!eof() && peek() != '\n') get();
    return { TokenType::PREPROCESSOR_DIRECTIVE, input.substr(start, pos - start) };
}

Token Lexer::readComment() {
    size_t start = pos;
    pos += 2;
    while (!eof() && peek() != '\n') get();
    return { TokenType::COMMENT, input.substr(start, pos - start) };
}

Token Lexer::readString() {
    size_t start = pos;
    bool closed = false;
    get();

    while (!eof()) {
        char ch = get();
        if (ch == '\\') { if (!eof()) get(); }
        else if (ch == '"') { closed = true; break; }
        else if (ch == '\n' || ch == '\0') break;
    }

    string val = input.substr(start, pos - start);
    return { closed ? TokenType::STRING_LITERAL : TokenType::UNKNOWN, val };
}



Token Lexer::readChar() {
    size_t start = pos;
    bool closed = false;
    get();

    if (!eof()) {
        if (peek() == '\\') { get(); if (!eof()) get(); }
        else get();

        if (!eof() && peek() == '\'') { get(); closed = true; }
    }

    string val = input.substr(start, pos - start);
    return { closed ? TokenType::CHAR_LITERAL : TokenType::UNKNOWN, val };
}



Token Lexer::readIdentifier() {
    size_t start = pos;
    if (peek() == '@') get();

    while (!eof() && isAlphaNum(peek())) get();

    if (!eof() && (peek() == '"' || peek() == '\'')) {
        char delim = get();
        skipUntil(delim);
        return { TokenType::UNKNOWN, input.substr(start, pos - start) };
    }

    string word = input.substr(start, pos - start);
    TokenType type = keywords.count(word) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
    return { type, word };
}

void Lexer::skipUntil(char delimiter) {
    while (!eof() && peek() != delimiter && !isWhitespace(peek())) {
        if (punctuators.find(peek()) != string::npos) break;
        get();
    }
    if (!eof() && peek() == delimiter) get();
}

Token Lexer::readDecimalOrFloat(size_t start) {
    bool isFloat = false;
    while (!eof() && isDigit(peek())) get();

    if (!eof() && peek() == '.') {
        isFloat = true;
        get();
        while (!eof() && isDigit(peek())) get();
    }

    if (!eof() && tolower(peek()) == 'e') {
        size_t save = pos;
        get();
        if (!eof() && (peek() == '+' || peek() == '-')) get();

        bool hasExpDigits = false;
        while (!eof() && isDigit(peek())) {
            get();
            hasExpDigits = true;
        }

        if (!hasExpDigits) pos = save;
        else isFloat = true;
    }

    consumeSuffixes();

    if (!eof() && (isAlpha(peek()) || peek() == '_')) {
        return handleInvalidSequence(start);
    }

    string val = input.substr(start, pos - start);
    return { isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL, val };
}

Token Lexer::readNumber() {
    size_t start = pos;

    if (peek() == '0' && tolower(peek(1)) == 'x') {
        return readHexLiteral(start);
    }
    return readDecimalOrFloat(start);
}

Token Lexer::readHexLiteral(size_t start) {
    pos += 2;
    bool hasDigits = false;
    while (!eof() && isHexDigit(peek())) { get(); hasDigits = true; }

    if (!hasDigits) return handleInvalidSequence(start);

    consumeSuffixes();
    return { TokenType::HEX_LITERAL, input.substr(start, pos - start) };
}

void Lexer::consumeSuffixes() {
    string suffixes = "fFdDmMuUlL";
    if (!eof() && suffixes.find(peek()) != string::npos) {
        char first = get();
        if (!eof() && suffixes.find(peek()) != string::npos && tolower(peek()) != tolower(first)) {
            get();
        }
    }
}

Token Lexer::handleInvalidSequence(size_t start) {
    while (!eof() && !isWhitespace(peek()) && punctuators.find(peek()) == string::npos) {
        get();
    }
    return { TokenType::UNKNOWN, input.substr(start, pos - start) };
}

Token Lexer::tryReadOperator() {
    if (pos + 2 < input.size()) {
        string three = input.substr(pos, 3);
        if (threeCharOps.count(three)) { pos += 3; return { TokenType::OPERATOR, three }; }
    }
    if (pos + 1 < input.size()) {
        string two = input.substr(pos, 2);
        if (twoCharOps.count(two)) { pos += 2; return { TokenType::OPERATOR, two }; }
    }
    if (singleCharOps.find(peek()) != string::npos) {
        string s(1, get());
        return { TokenType::OPERATOR, s };
    }
    return { TokenType::UNKNOWN, "" };
}

Token Lexer::tryReadPunctuator() {
    if (punctuators.find(peek()) != string::npos) {
        string s(1, get());
        return { TokenType::PUNCTUATOR, s };
    }
    return { TokenType::UNKNOWN, "" };
}