//fix hex nubmbers can contain letters after F
//<2abcp, INTEGER_LITERAL>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cctype>
using namespace std;

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
	string value;
	Token(TokenType t, string v) : type(t), value(std::move(v)) {}
};

string tokenTypeName(TokenType t) {
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

class Lexer {
	string input;
	size_t pos = 0;
	bool atLineStart = true; // для директив препроцесора

	unordered_set<string> keywords;
	unordered_set<string> twoCharOps;
	unordered_set<string> threeCharOps;
	string singleCharOps = "+-*/%=&|!<>^~";
	string punctuators = "();{}[],.:?";

public:
	Lexer(string s) : input(std::move(s)) {
		initKeywords();
		initOps();
	}

	void initKeywords() {
		string ks[] = {
			"abstract","as","base","bool","break","byte","case","catch","char","checked",
			"class","const","continue","decimal","default","delegate","do","double","else",
			"enum","event","explicit","extern","false","finally","fixed","float","for","foreach",
			"goto","if","implicit","in","int","interface","internal","is","lock","long",
			"namespace","new","null","object","operator","out","override","params","private",
			"protected","public","readonly","ref","return","sbyte","sealed","short","sizeof",
			"stackalloc","static","string","struct","switch","this","throw","true","try","typeof",
			"uint","ulong","unchecked","unsafe","ushort","using","virtual","void","volatile","while",
			"using" // repeated harmlessly
		};
		for (auto& k : ks) keywords.insert(k);
	}

	void initOps() {
		string t2[] = { "==","<=",">=","!=","&&","||","++","--","+=", "-=", "*=", "/=", "%=",
					   "&=", "|=", "^=", "->", "??", "::","<<", ">>", "=>" };
		string t3[] = { "<<=", ">>=" };
		for (auto& s : t2) twoCharOps.insert(s);
		for (auto& s : t3) threeCharOps.insert(s);
	}

	//to check whether we reached the end of input
	bool eof() const { return pos >= input.size(); }
	// lookahead
	char peek(size_t offset = 0) const { return (pos + offset < input.size()) ? input[pos + offset] : '\0'; }
	// consume current char and advance
	char get() { return eof() ? '\0' : input[pos++]; }
	void advance(size_t n) { pos = min(pos + n, input.size()); }

	bool isAlpha(char c) const { return (c == '_' || isalpha(static_cast<unsigned char>(c))); }
	bool isDigit(char c) const { return isdigit(static_cast<unsigned char>(c)); }
	bool isHexDigit(char c) const { return isxdigit(static_cast<unsigned char>(c)); }
	bool isAlphaNum(char c) const { return isAlpha(c) || isDigit(c); }

	vector<Token> tokenize() {
		vector<Token> tokens;
		while (!eof()) {
			char c = peek();

			// whitespace handling (update atLineStart)
			if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
				if (c == '\n') atLineStart = true;
				get();
				continue;
			}

			// Preprocessor directive: '#' at start of line
			if (c == '#' && atLineStart) {
				size_t start = pos;
				// read to end of line
				while (!eof() && peek() != '\n') get();
				string val = input.substr(start, pos - start);
				tokens.emplace_back(TokenType::PREPROCESSOR_DIRECTIVE, val);
				atLineStart = true; // newline not consumed yet or was earlier
				continue;
			}

			// Single-line comment //
			if (c == '/' && peek(1) == '/') {
				size_t start = pos;
				pos += 2;
				while (!eof() && peek() != '\n') get();
				string val = input.substr(start, pos - start);
				tokens.emplace_back(TokenType::COMMENT, val);
				// atLineStart will be set when we consume newline on next loop
				continue;
			}

			// String literal "
			if (c == '"') {
				size_t start = pos;
				bool closed = false;
				get(); // consume "
				while (!eof()) {
					char ch = get();
					if (ch == '\\') {
						// escape, skip next char if exists
						if (!eof()) get();
					}
					else if (ch == '"') {
						closed = true;
						break;
					}
					else if (ch == '\n' || ch == '\0') {
						break; // newline inside string -> unterminated
					}
				}
				string val = input.substr(start, pos - start);
				if (closed) tokens.emplace_back(TokenType::STRING_LITERAL, val);
				else tokens.emplace_back(TokenType::UNKNOWN, val); // помилка: незакритий рядок
				continue;
			}

			// Char literal '
			if (c == '\'') {
				size_t start = pos;
				bool closed = false;
				get(); // consume '
				if (!eof()) {
					if (peek() == '\\') {
						get(); // backslash
						if (!eof()) get(); // escaped char
					}
					else {
						get(); // char
					}
					if (!eof() && peek() == '\'') {
						get(); // closing '
						closed = true;
					}
				}
				string val = input.substr(start, pos - start);
				if (closed) tokens.emplace_back(TokenType::CHAR_LITERAL, val);
				else tokens.emplace_back(TokenType::UNKNOWN, val); // помилка
				continue;
			}

			// Identifier or keyword (starts with letter or _)
			if (isAlpha(c) || c == '@') {
				size_t start = pos;
				get(); // consume '@'
				while (!eof() && (isAlphaNum(peek()) || peek() == '_')) get();
				string word = input.substr(start, pos - start);
				if (keywords.count(word)) tokens.emplace_back(TokenType::KEYWORD, word);
				else tokens.emplace_back(TokenType::IDENTIFIER, word);
				atLineStart = false;
				continue;
			}

			// Number: hex 0x... OR decimal / float (with optional exponent)
			if (isDigit(c)) {
				size_t start = pos;
				// hex?
				if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X')) {
					pos += 2;
					bool hasHex = false;
					while (!eof() && isHexDigit(peek())) {
						hasHex = true;
						get();
					}
					// optional integer suffixes (u,l,ul,LU etc) - we'll just consume letters if present
					while (!eof() && isalpha((unsigned char)peek())) get();
					string val = input.substr(start, pos - start);
					if (hasHex) tokens.emplace_back(TokenType::HEX_LITERAL, val);
					else tokens.emplace_back(TokenType::UNKNOWN, val);
					atLineStart = false;
					continue;
				}
				// decimal or float
				bool isFloat = false;
				while (!eof() && isDigit(peek())) get();
				if (!eof() && peek() == '.') {
					// could be float
					isFloat = true;
					get();
					while (!eof() && isDigit(peek())) get();
				}
				// exponent part
				if (!eof() && (peek() == 'e' || peek() == 'E')) {
					size_t save = pos;
					get();
					if (!eof() && (peek() == '+' || peek() == '-')) get();
					bool expDigits = false;
					while (!eof() && isDigit(peek())) { get(); expDigits = true; }
					if (!expDigits) pos = save; else isFloat = true;
				}
				// suffix letters (f, d, m, u, l etc) - consume letters
				while (!eof() && isalpha((unsigned char)peek())) get();
				string val = input.substr(start, pos - start);
				tokens.emplace_back(isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL, val);
				atLineStart = false;
				continue;
			}

			// Operators: try three-char, then two-char, then single-char
			// try three-char
			if (pos + 2 < input.size()) {
				string three = input.substr(pos, 3);
				if (threeCharOps.count(three)) {
					tokens.emplace_back(TokenType::OPERATOR, three);
					pos += 3;
					atLineStart = false;
					continue;
				}
			}
			// two-char
			if (pos + 1 < input.size()) {
				string two = input.substr(pos, 2);
				if (twoCharOps.count(two)) {
					tokens.emplace_back(TokenType::OPERATOR, two);
					pos += 2;
					atLineStart = false;
					continue;
				}
			}
			// single-char operator
			if (singleCharOps.find(c) != string::npos) {
				string s(1, c);
				tokens.emplace_back(TokenType::OPERATOR, s);
				get();
				atLineStart = false;
				continue;
			}

			// punctuators
			if (punctuators.find(c) != string::npos) {
				string s(1, c);
				tokens.emplace_back(TokenType::PUNCTUATOR, s);
				get();
				atLineStart = false;
				continue;
			}

			// If none matched -> unknown token (single char)
			{
				string s(1, c);
				tokens.emplace_back(TokenType::UNKNOWN, s);
				get();
				atLineStart = false;
			}
		}
		return tokens;
	}
};

int main(int argc, char** argv) {
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	string source;
	if (argc > 1) {
		// read file
		ifstream in(argv[1]);
		if (!in.is_open()) {
			cerr << "Cannot open file: " << argv[1] << "\n";
			return 1;
		}
		std::ostringstream ss;
		ss << in.rdbuf();
		source = ss.str();
	}
	else {
		// read from stdin
		std::ostringstream ss;
		ss << cin.rdbuf();
		source = ss.str();
	}

	Lexer lexer(source);
	auto tokens = lexer.tokenize();

	for (const auto& t : tokens) {
		cout << "<" << t.value << ", " << tokenTypeName(t.type) << ">\n";
	}

	return 0;
}
