#pragma once
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include "Lexer.h"

class LexerFixture : public ::testing::Test {
protected:
    std::unique_ptr<Lexer> lexer;
    void SetUp() override { lexer = std::make_unique<Lexer>(""); }
};