#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#include "error.h"
#include "Token.h"
#include "Lexer.h"

using namespace std;

Lexer::Lexer(string filePath)
{
    ifstream file(filePath);
    if (!file.is_open())
        error("Could not open or find file " + filePath);

    string line;
    while (getline(file, line))
    {
        source.append(line);
        source.append("\n");
    }
    source.append("\0");
}

void Lexer::advance()
{
    if (source[current] == '\n')
    {
        lineNumber++;
        columnNumber = 1;
    }
    else
        columnNumber++;
    current++;
}

char Lexer::nextChar()
{
    advance();
    return source[current - 1];
}

char Lexer::peek()
{
    return source[current];
}

char Lexer::peekNext()
{
    return source[current + 1];
}

bool Lexer::match(char c)
{
    if (peek() == c)
    {
        advance();
        return true;
    }
    return false;
}

void Lexer::eat(char c)
{
    if (!match(c))
    {
        stringstream message;
        message << "Expected " << c << " but got " << peek();
        error(message.str(), newToken(TkType::UNKNOWN_TOKEN));
    }
}

void Lexer::eatWhitespace()
{
    char c;
    while (true)
    {
        c = peek();
        if (c != '\n' && c != ' ' && c != '\t')
            break;
        advance();
    }
}

void Lexer::eatComments()
{
    while (true)
    {
        if (peek() == '/' && peekNext() == '/')
        {
            advance();
            while (peek() != '\n')
                advance();
            advance();
        }
        else if (peek() == '/' && peekNext() == '*')
        {
            advance();
            while (!(peek() == '*' && peekNext() == '/'))
                advance();
            advance();
            advance();
        }
        //TODO: Add support for nested Block Comments
        else
            break;

        eatWhitespace();
    }
}

bool Lexer::isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isNumber(char c)
{
    return c >= '0' && c <= '9';
}

Token Lexer::lexKeywordOrIdentifier()
{
    while (isAlpha(peek()) || isNumber(peek()))
        advance();

    Token tk = newToken(TkType::IDENTIFIER);

    for (int i = 0; i < KEYWORDS.size(); i++)
    {
        auto tuple = KEYWORDS[i];
        if (get<0>(tuple) == tk.source)
            return newToken(get<1>(tuple));
    }

    return tk;
}

Token Lexer::lexNumberConstant()
{
    bool isDouble = false;
    while (isNumber(peek()))
        advance();
    if (peek() == '.' && peekNext() != '.')
    {
        advance();
        isDouble = true;
    }
    while (isNumber(peek()))
        advance();

    return newToken(isDouble ? TkType::DOUBLE_CONSTANT : TkType::NUMBER_CONSTANT);
}

Token Lexer::lexStringConstant()
{
    int currentLineNumber = lineNumber;
    while (peek() != '"')
        advance();
    eat('\"');

    if (currentLineNumber != lineNumber)
        error("String Constants need to end on the same line they're started on.", newToken(TkType::UNKNOWN_TOKEN));

    return newToken(TkType::STRING_CONSTANT);
}

Token Lexer::nextToken()
{
    eatWhitespace();
    eatComments();

    start = current;
    char c = nextChar();

    switch (c)
    {
    case 0:
        return newToken(TkType::END);
    case '=':
        return newToken(match('=') ? TkType::EQUAL_EQUAL : TkType::EQUAL);
    case '+':
        return newToken(match('=') ? TkType::PLUS_EQUAL : TkType::PLUS);
    case '-':
        return newToken(match('=') ? TkType::MINUS_EQUAL : match('>') ? TkType::ARROW
                                                                      : TkType::MINUS);
    case '/':
        return newToken(match('=') ? TkType::SLASH_EQUAL : TkType::SLASH);
    case '*': return newToken(match('=') ? TkType::STAR_EQUAL : TkType::STAR);
    case '!': return newToken(match('=') ? TkType::BANG_EQUAL : TkType::BANG);
    case '<': return newToken(match('=') ? TkType::LESS_EQUAL : TkType::LESS);
    case '>': return newToken(match('=') ? TkType::GREATER_EQUAL : TkType::GREATER);
    case ':': return newToken(match(':') ? TkType::COLON_COLON : TkType::COLON);
    case '|': return newToken(match('|') ? TkType::OR : TkType::BIT_OR);
    case '&': return newToken(match('&') ? TkType::AND : TkType::BIT_AND);
    case '.':
        return newToken(match('.') ? TkType::POINT_POINT : TkType::POINT);
    case ',':
        return newToken(TkType::COMMA);
    case ';':
        return newToken(TkType::SEMICOLON);
    case '(':
        return newToken(TkType::OPEN_PAREN);
    case ')':
        return newToken(TkType::CLOSE_PAREN);
    case '{':
        return newToken(TkType::OPEN_CURLY);
    case '}':
        return newToken(TkType::CLOSE_CURLY);
    case '[':
        return newToken(TkType::OPEN_BRACKET);
    case ']':
        return newToken(TkType::CLOSE_BRACKET);
    case '#':
        return lexKeywordOrIdentifier();
    case '"':
        return lexStringConstant();
    default:
        if (isNumber(c))
            return lexNumberConstant();
        if (isAlpha(c))
            return lexKeywordOrIdentifier();
        return newToken(TkType::UNKNOWN_TOKEN);
    }
}

Token Lexer::newToken(TkType type)
{
    string s(source, start, current - start);

    Token tk(type, s, lineNumber, columnNumber);
    return tk;
}
