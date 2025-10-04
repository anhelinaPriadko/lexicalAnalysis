#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>
using namespace std;

class Lexer {
    string input;
    size_t pos = 0;
    bool atLineStart = true;

    unordered_set<string> keywords;
    unordered_set<string> twoCharOps;
    unordered_set<string> threeCharOps;
    string singleCharOps = "+-*/%=&|!<>^~";
    string punctuators = "();{}[],.:?";

public:
    explicit Lexer(string s) : input(std::move(s)) {
        initKeywords();
        initOps();
    }

    vector<Token> tokenize();

private:
    bool eof() const { return pos >= input.size(); }
    char peek(size_t offset = 0) const { return (pos + offset < input.size()) ? input[pos + offset] : '\0'; }
    char get() { return eof() ? '\0' : input[pos++]; }

    bool isWhitespace(char c) const { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
    bool isAlpha(char c) const { return (c == '_' || isalpha(static_cast<unsigned char>(c))); }
    bool isDigit(char c) const { return isdigit(static_cast<unsigned char>(c)); }
    bool isHexDigit(char c) const { return isxdigit(static_cast<unsigned char>(c)); }
    bool isAlphaNum(char c) const { return isAlpha(c) || isDigit(c); }

    void handleWhitespace();
    void initKeywords();
    void initOps();

    Token readPreprocessor();
    Token readComment();
    Token readString();
    Token readChar();
    Token readIdentifier();
    Token readNumber();
    Token tryReadOperator();
    Token tryReadPunctuator();
};
