#pragma once

#include <vector>
#include <string>

#include "Token.h"
#include "Lexer.h"
#include "Stmt.h"
#include "Expr.h"

using namespace std;

struct Parser {
    Lexer lx;
    Token tk;
    Token prevTk;

    vector<Stmt *> statements;
    vector<Decl *> declarations;

    Parser(const char* filePath);

    OP toOperand(Token tk);

    string* toString(Token &tk);

    bool toBool(Token &tk);

    char toChar(Token &tk);

    long long int toInt(Token &tk);

    double toDouble(Token &tk);

    Token nextToken();

    bool match(TkType type);

    template<typename... Args>
    bool match(TkType type, Args... args)
    {
        if (match(type)) return true;
        return match(args...);
    }

#define CHECK(t) \
    if (tk.type != t) { \
        vector<string> expected = { TkTypetoString(t) }; \
        ERROR(prevTk.source, expected, tk); \
    } \

#define CONSUME(type) \
    CHECK(type);    \
    nextToken();    

    Type getType(string& s);

    Expr* primary();

    Expr* call();

    Expr* parseArguments(Expr* callee);

    Expr* unary();

    Expr* factor();

    Expr* term();

    Expr* comparison();

    Expr* equality();

    Expr* logicAnd();

    Expr* logicOr();

    Expr* parseExpression();

    Stmt* parseFunctionDefinition(string &name);

    Block* parseBlock();

    Decl* parseFunctionParameter();

    Stmt* parseStruct(string &name);

    Stmt* parseEnum(string name);

    Stmt* parseWhileStatement();

    Stmt* parseForStatement();

    Stmt* parseDeferStatement();

    Stmt* parseIfStatement();

    Stmt* parseElseStatement();

    Stmt* parseReturnStatement();

    Stmt* parseIdentifierStatement();

    Stmt* parseStatement();

    void parse();

    string loadFile(string file);
};