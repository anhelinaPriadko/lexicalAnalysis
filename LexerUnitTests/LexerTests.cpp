//#include <gtest/gtest.h>
//#include <gmock/gmock.h>
//#include <memory>
//#include <vector>
//#include <string>
//#include "LexerFixture.h"
//#include "FileReader.h"
//#include "Lexer.h"
//#include "Token.h"
//
//using namespace std;
//using namespace testing;
//
//TEST_F(LexerFixture, BasicTokenization) {
//    lexer = std::make_unique<Lexer>("int x = 5;");
//    auto tokens = lexer->tokenize();
//
//    ASSERT_EQ(tokens.size(), 5);
//    EXPECT_TRUE(tokens[0].type == TokenType::KEYWORD);
//    EXPECT_STREQ(tokens[1].value.c_str(), "x");
//    EXPECT_FALSE(tokens.empty());
//}
//
//TEST_F(LexerFixture, StringLiterals) {
//    lexer = std::make_unique<Lexer>("\"Hello \\\"World\\\"\" \"unclosed");
//    auto tokens = lexer->tokenize();
//
//    ASSERT_EQ(tokens.size(), 2);
//    EXPECT_EQ(tokens[0].type, TokenType::STRING_LITERAL);
//    EXPECT_EQ(tokens[1].type, TokenType::UNKNOWN);
//}
//
//TEST_F(LexerFixture, CharLiterals) {
//    lexer = std::make_unique<Lexer>("'a' '\\n' 'x");
//        auto tokens = lexer->tokenize();
//
//    ASSERT_EQ(tokens.size(), 3);
//    EXPECT_EQ(tokens[0].type, TokenType::CHAR_LITERAL);
//    EXPECT_EQ(tokens[1].type, TokenType::CHAR_LITERAL);
//    EXPECT_EQ(tokens[2].type, TokenType::UNKNOWN);
//}
//
//TEST_F(LexerFixture, NumberLiterals) {
//    lexer = std::make_unique<Lexer>("123 456U 789L 123UL");
//    auto tokens = lexer->tokenize();
//
//    ASSERT_EQ(tokens.size(), 4);
//    for (const auto& t : tokens) {
//        EXPECT_EQ(t.type, TokenType::INTEGER_LITERAL);
//    }
//}
//
//TEST_F(LexerFixture, HexLiterals) {
//    lexer = std::make_unique<Lexer>("0xABC 0x123UL 0x 0xG1");
//    auto tokens = lexer->tokenize();
//
//    ASSERT_EQ(tokens.size(), 4);
//    EXPECT_EQ(tokens[0].type, TokenType::HEX_LITERAL);
//    EXPECT_EQ(tokens[1].type, TokenType::HEX_LITERAL);
//    EXPECT_EQ(tokens[2].type, TokenType::UNKNOWN);
//    EXPECT_EQ(tokens[3].type, TokenType::UNKNOWN);
//}
//
//TEST_F(LexerFixture, FloatLiterals) {
//    lexer = std::make_unique<Lexer>("3.14 1e10 1.5f 2.0d 10.5m 1.2e-5");
//    auto tokens = lexer->tokenize();
//
//    ASSERT_EQ(tokens.size(), 6);
//    for (const auto& t : tokens) {
//        EXPECT_EQ(t.type, TokenType::FLOAT_LITERAL);
//    }
//}
//
//TEST_F(LexerFixture, IdentifiersAndKeywords) {
//    lexer = std::make_unique<Lexer>("@variable MyClass class @\"bad\"");
//    auto tokens = lexer->tokenize();
//
//    ASSERT_EQ(tokens.size(), 4);
//    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
//    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
//    EXPECT_EQ(tokens[2].type, TokenType::KEYWORD);
//    EXPECT_EQ(tokens[3].type, TokenType::UNKNOWN);
//}
//
//TEST_F(LexerFixture, OperatorsAndPunctuators) {
//    lexer = std::make_unique<Lexer>("<<= ++ == -> ( ) { }");
//    auto tokens = lexer->tokenize();
//
//    vector<string> values;
//    for (const auto& t : tokens) values.push_back(t.value);
//
//    EXPECT_THAT(values, ElementsAre("<<=", "++", "==", "->", "(", ")", "{", "}"));
//    EXPECT_EQ(tokens[0].type, TokenType::OPERATOR);
//    EXPECT_EQ(tokens[4].type, TokenType::PUNCTUATOR);
//}
//
//TEST_F(LexerFixture, CommentsAndPrepro) {
//    lexer = std::make_unique<Lexer>("#region\n// comment\n#endregion");
//    auto tokens = lexer->tokenize();
//
//    ASSERT_GE(tokens.size(), 3);
//    EXPECT_EQ(tokens[0].type, TokenType::PREPROCESSOR_DIRECTIVE);
//    EXPECT_EQ(tokens[1].type, TokenType::COMMENT);
//}
//
//TEST_F(LexerFixture, ExceptionTesting) {
//    char* argv[] = { (char*)"prog", (char*)"non_existent.cs" };
//    EXPECT_THROW(readSource(2, argv), std::runtime_error);
//}
//
//class ParameterizedKeywordTest : public ::testing::TestWithParam<const char*> {};
//
//TEST_P(ParameterizedKeywordTest, KeywordsCheck) {
//    Lexer paramLexer(GetParam());
//    auto tokens = paramLexer.tokenize();
//    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD);
//}
//
//INSTANTIATE_TEST_SUITE_P(
//    CsharpKeywords,
//    ParameterizedKeywordTest,
//    ::testing::Values("abstract", "bool", "catch", "double", "finally", "interface", "lock", "object", "sealed", "volatile")
//);