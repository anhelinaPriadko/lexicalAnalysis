#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Lexer.h"
#include "MockFileReader.h"

using namespace testing;

namespace MockTests
{
    TEST(LexerMockTest, SequenceAndDifferentReturns) {
        MockFileReader mock;
        Lexer lexer(mock);

        InSequence seq;

        EXPECT_CALL(mock, exists("config.cs")).WillOnce(Return(true));
        EXPECT_CALL(mock, read("config.cs")).WillOnce(Return(""));

        EXPECT_CALL(mock, exists("config.cs")).WillOnce(Return(true));
        EXPECT_CALL(mock, read("config.cs")).WillOnce(Return("int x = 5;"));

        lexer.loadFile("config.cs");
        lexer.loadFile("config.cs");
    }

    TEST(LexerMockTest, ExceptionHandlingScenario) {
        MockFileReader mock;
        Lexer lexer(mock);

        EXPECT_CALL(mock, exists("secret.cs"))
            .WillOnce(Throw(std::runtime_error("Access Denied")));

        EXPECT_THROW(lexer.loadFile("secret.cs"), std::runtime_error);
    }

    TEST(LexerMockTest, ComplexMatchersAndCallCount) {
        MockFileReader mock;
        Lexer lexer(mock);

        EXPECT_CALL(mock, exists(AnyOf(EndsWith(".cs"), EndsWith(".txt"))))
            .Times(2)
            .WillRepeatedly(Return(true));

        EXPECT_CALL(mock, read(_)).Times(2).WillRepeatedly(Return("var a = 1;"));

        EXPECT_CALL(mock, exists(Not(AnyOf(EndsWith(".cs"), EndsWith(".txt")))))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(false));

        lexer.loadFile("main.cs");
        lexer.loadFile("notes.txt");
        lexer.loadFile("image.png");
    }
}