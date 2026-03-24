#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Lexer.h"
#include "MockFileReader.h"

using namespace testing;

TEST(LexerMockTest, Lab2RequirementsChain) {
    MockFileReader mock;
    Lexer lexer(mock);

    InSequence seq;

    EXPECT_CALL(mock, exists(EndsWith(".cs")))
        .WillOnce(Return(true));
    EXPECT_CALL(mock, read(_))
        .WillOnce(Return(""));

    EXPECT_CALL(mock, exists(EndsWith(".cs")))
        .WillOnce(Return(true));
    EXPECT_CALL(mock, read(_))
        .WillOnce(Return("int x = 10;"));

    EXPECT_CALL(mock, exists("secret.cs"))
        .WillOnce(Throw(std::runtime_error("Access Denied by System")));

    lexer.loadFile("program.cs");
    lexer.loadFile("another.cs");

    EXPECT_THROW(lexer.loadFile("secret.cs"), std::runtime_error);
}

TEST(LexerMockTest, CallCountVerification) {
    MockFileReader mock;
    Lexer lexer(mock);

    EXPECT_CALL(mock, exists(_))
        .Times(AtMost(3))
        .WillRepeatedly(Return(false));

    lexer.loadFile("file1.cs");
    lexer.loadFile("file2.cs");
}