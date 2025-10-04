C# Lexer in C++

This project implements a lexical analyzer (lexer) for the C# language in C++.
The lexer tokenizes C# code into tokens and classifies them by type: keywords, identifiers, numbers, strings, characters, operators, punctuators, comments, and preprocessor directives.

Features:

1.Supports C# keywords, including literal keywords (true, false, null).

2.Recognizes identifiers and qualified names (e.g., Console.WriteLine).

3.Supports integers, floating-point, and hexadecimal numbers, including suffixes (f, u, l).

4.Handles string and character literals, including escape sequences (\n, \\, \").

5.Supports single-line and multi-line comments.

6.Recognizes operators and punctuators, including combined forms like ++, +=, &&, ||, <<, >>.

7.Detects unfinished strings and characters as UNKNOWN.

8.Simple code reading from file input or standard input.

Project Structure:

1.Token.h — token and token type definitions.

2.Lexer.h / Lexer.cpp — Lexer class and its methods for tokenization.

3.Fileader.h / FileReader.cpp — functions for reading file with code to analize.

4.main.cpp — example usage of the lexer and token output.
