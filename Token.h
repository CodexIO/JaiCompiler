#pragma once

#include <string>

using namespace std;

enum TkType {
    LOAD,
    STRUCT,
    ENUM,
    DEFER,
    FOR,
    WHILE,
    BREAK,
    CONTINUE,
    
    IF,
    ELSE,
    THEN,
    RETURN,
    TYPE,

    NULL_CONSTANT,
    CHAR_CONSTANT,
    NUMBER_CONSTANT,
    DOUBLE_CONSTANT,
    STRING_CONSTANT,
    BOOL_CONSTANT,
    IDENTIFIER,

    EQUAL,
    EQUAL_EQUAL,
    BANG,
    BANG_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    OR,
    AND,
    BIT_OR,
    BIT_AND,

    COMMA,
    POINT,
    POINT_POINT,
    COLON,
    COLON_COLON,
    SEMICOLON,
    ARROW,

    PLUS,
    PLUS_EQUAL,
    MINUS,
    MINUS_EQUAL,
    SLASH,
    SLASH_EQUAL,
    STAR,
    STAR_EQUAL,


    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_CURLY,
    CLOSE_CURLY,
    OPEN_BRACKET,
    CLOSE_BRACKET,

    UNKNOWN_TOKEN,
    END
};

string TkTypetoString(TkType type);

ostream& operator<<(ostream& out, const TkType type);

struct Token {
    TkType type;
    std::string source;
    int lineNumber;
    int coulmnNumber;

    Token(TkType type, std::string source, int lN, int cN);

    Token();

    void print();
};