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
    string ks[] = {
            "abstract","as","base","bool","break","byte","case","catch","char","checked",
            "class","const","continue","decimal","default","delegate","do","double","else",
            "enum","event","explicit","extern","false","finally","fixed","float","for","foreach",
            "goto","if","implicit","in","int","interface","internal","is","lock","long",
            "namespace","new","null","object","operator","out","override","params","private",
            "protected","public","readonly","ref","return","sbyte","sealed","short","sizeof",
            "stackalloc","static","string","struct","switch","this","throw","true","try","typeof",
            "uint","ulong","unchecked","unsafe","ushort","using","virtual","void","volatile","while"
    };
    for (auto& k : ks) keywords.insert(k);
}

void Lexer::initOps() {
    string t2[] = { "==","<=",">=","!=","&&","||","++","--","+=", "-=", "*=", "/=", "%=",
                        "&=", "|=", "^=", "->", "??", "::","<<", ">>", "=>" };
    string t3[] = { "<<=", ">>=" };
    for (auto& s : t2) twoCharOps.insert(s);
    for (auto& s : t3) threeCharOps.insert(s);
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
        get();
        if (input[pos - 1] == '"') {
            while (!eof() && peek() != '"' && !isWhitespace(peek()) &&
                (punctuators.find(peek()) == string::npos)) {
                get();
            }
            if (!eof() && peek() == '"') get();
        }
        else {
            while (!eof() && peek() != '\'' && !isWhitespace(peek()) &&
                (punctuators.find(peek()) == string::npos)) {
                get();
            }
            if (!eof() && peek() == '\'') get();
        }
        string bad = input.substr(start, pos - start);
        return { TokenType::UNKNOWN, bad };
    }

    string word = input.substr(start, pos - start);
    return { keywords.count(word) ? TokenType::KEYWORD : TokenType::IDENTIFIER, word };
}


Token Lexer::readNumber() {
    size_t start = pos;

    if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
        pos += 2;
        bool hasHex = false;

        while (!eof() && isHexDigit(peek())) {
            get();
            hasHex = true;
        }

        if (!hasHex) {
            while (!eof() && !isWhitespace(peek()) &&
                (punctuators.find(peek()) == string::npos) &&
                singleCharOps.find(peek()) == string::npos) {
                get();
            }
            return { TokenType::UNKNOWN, input.substr(start, pos - start) };
        }

        if (!eof()) {
            char ch = peek();
            if (ch == 'u' || ch == 'U' || ch == 'l' || ch == 'L') {
                get();
                if (!eof()) {
                    char ch2 = peek();
                    if ((ch2 == 'u' || ch2 == 'U' || ch2 == 'l' || ch2 == 'L') &&
                        (tolower(ch2) != tolower(ch))) {
                        get();
                    }
                }
                if (!eof() && (isAlpha(peek()) || peek() == '_')) {
                    while (!eof() && (isAlphaNum(peek()) || peek() == '_')) get();
                    return { TokenType::UNKNOWN, input.substr(start, pos - start) };
                }
                return { TokenType::HEX_LITERAL, input.substr(start, pos - start) };
            }

            if (isAlpha(peek()) || peek() == '_') {
                while (!eof() && (isAlphaNum(peek()) || peek() == '_')) get();
                return { TokenType::UNKNOWN, input.substr(start, pos - start) };
            }
        }

        return { TokenType::HEX_LITERAL, input.substr(start, pos - start) };
    }

    bool isFloat = false;
    while (!eof() && isDigit(peek())) get();

    if (!eof() && peek() == '.') {
        isFloat = true;
        get();
        while (!eof() && isDigit(peek())) get();
    }

    if (!eof() && (peek() == 'e' || peek() == 'E')) {
        size_t save = pos;
        get();
        if (!eof() && (peek() == '+' || peek() == '-')) get();

        bool expDigits = false;
        while (!eof() && isDigit(peek())) { get(); expDigits = true; }

        if (!expDigits) pos = save; else isFloat = true;
    }

    if (!eof()) {
        char ch = peek();
        if (string("fFdDmMuUlL").find(ch) != string::npos) {
            get();
            if (!eof()) {
                char ch2 = peek();
                if ((ch2 == 'u' || ch2 == 'U' || ch2 == 'l' || ch2 == 'L') &&
                    (string("fFdDmM").find(ch) == string::npos)) {
                    get();
                }
            }
            if (!eof() && (isAlpha(peek()) || peek() == '_')) {
                while (!eof() && (isAlphaNum(peek()) || peek() == '_')) get();
                return { TokenType::UNKNOWN, input.substr(start, pos - start) };
            }
        }
        else if (isAlpha(peek()) || peek() == '_') {
            while (!eof() && (isAlphaNum(peek()) || peek() == '_')) get();
            return { TokenType::UNKNOWN, input.substr(start, pos - start) };
        }
    }

    string val = input.substr(start, pos - start);
    return { isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL, val };
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