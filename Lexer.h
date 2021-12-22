#pragma once

#include<vector>
#include<tuple>

#include "Token.h"

using namespace std;

const vector<tuple<string, TkType>> KEYWORDS{
    make_tuple("#load",     LOAD),
    make_tuple("struct",    TkType::STRUCT),
    make_tuple("enum",      TkType::ENUM),
    make_tuple("defer",     DEFER),
    make_tuple("for",       FOR),
    make_tuple("while",     WHILE),
    make_tuple("break",     BREAK),
    make_tuple("continue",  CONTINUE),
    make_tuple("#char",     CHAR_CONSTANT),
    make_tuple("if",        IF),
    make_tuple("else",      ELSE),
    make_tuple("then",      THEN),
    make_tuple("return",    RETURN),
    make_tuple("true",      BOOL_CONSTANT),
    make_tuple("false",     BOOL_CONSTANT),
    make_tuple("s64",       TYPE),
    make_tuple("s32",       TYPE),
    make_tuple("s16",       TYPE),
    make_tuple("s8",        TYPE),
    make_tuple("u64",       TYPE),
    make_tuple("u32",       TYPE),
    make_tuple("u16",       TYPE),
    make_tuple("u8",        TYPE),
    make_tuple("string",    TYPE),
    make_tuple("float",     TYPE),
    make_tuple("double",    TYPE),
    make_tuple("int",       TYPE),
    make_tuple("bool",      TYPE),
    make_tuple("void",      TYPE),
    make_tuple("null",      NULL_CONSTANT)
};

struct Lexer
{
    string source;
    int lineNumber = 1;
    int columnNumber = 1;
    
    int current = 0;
    int start = 0;

    Lexer(string filePath);

    void advance();

    char nextChar();

    char peek();

    char peekNext();

    bool match(char c);

    void eat(char c);

    void eatWhitespace();

    void eatComments();

    bool isAlpha(char c);

    bool isNumber(char c);

    Token lexKeywordOrIdentifier();

    Token lexNumberConstant();

    Token lexStringConstant();

    Token nextToken();

    Token newToken(TkType type);
};