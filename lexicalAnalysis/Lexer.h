#pragma once
#include "Token.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <cctype>
#include "IFileReader.h"
using namespace std;

class Lexer {
    IFileReader* fileReader = nullptr;
    std::string source;
    string input;
    size_t pos = 0;
    bool atLineStart = true;

    unordered_set<string> keywords;
    unordered_set<string> twoCharOps;
    unordered_set<string> threeCharOps;
    string singleCharOps = "+-*/%=&|!<>^~";
    string punctuators = "();{}[],.:?";

public:
    explicit Lexer(string s) : input(std::move(s)), fileReader(nullptr) {
        initKeywords();
        initOps();
    }

    explicit Lexer(IFileReader& reader) : fileReader(&reader), input("") {
        initKeywords();
        initOps();
    }

    void loadFile(const string& path) {
        if (fileReader != nullptr) {
            if (fileReader->exists(path)) {
                input = fileReader->read(path);
                pos = 0;
                atLineStart = true;
            }
        }
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

    void skipUntil(char delimiter);
    Token handleInvalidSequence(size_t start);

    Token readHexLiteral(size_t start);
    Token readDecimalOrFloat(size_t start);
    void consumeSuffixes();

    Token readPreprocessor();
    Token readComment();
    Token readString();
    Token readChar();
    Token readIdentifier();
    Token readNumber();
    Token tryReadOperator();
    Token tryReadPunctuator();
};
