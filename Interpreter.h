#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <stack>

#include "Stmt.h"
#include "Expr.h"
#include "Parser.h"

using std::string, std::cout, std::endl, std::unordered_map, std::vector;

struct Interpreter {
    Parser parser;
    unordered_map<string, Any> variables{0};
    unordered_map<string, Any> constants{0};
    unordered_map<string, Struct*> structs{0};
    unordered_map<string, Enum*> enums{0};
    unordered_map<string, Func*> functions{0};
    stack<vector<Stmt*>> deferStatements;

    Any returnValue;
    bool shouldReturn = false;
    bool shouldContinue = false;
    bool shouldBreak = false;
    Func* main = nullptr;

    Interpreter(Parser &p);

    void run();

    void setUpTables(Block* st);
    void setUpTables(Stmt* stmt);

    void inferTypes(vector<Decl*> decls);

    void callPrintf(Func* func);
    Any callFunction(Func* func);

    void pushDefers();
    void executeDefers();

    bool isTruthy(Any any);

    void runStmt(Stmt* stmt);
    void runDecl(Stmt* stmt);
    void runBlock(Stmt* stmt);
    void runStruct(Stmt* stmt);
    void runEnum(Stmt* stmt);
    void runFunc(Stmt* stmt);
    void runReturn(Stmt* stmt);
    void runIf(Stmt* stmt);
    void runElse(Stmt* stmt);
    void runExprStmt(Stmt* stmt);
    void runDefer(Stmt* stmt);
    void runFor(Stmt* stmt);
    void runWhile(Stmt* stmt);
    void runAssign(Stmt* stmt);
    void runSet(Stmt* stmt);
    void runContinue(Stmt* stmt);
    void runBreak(Stmt* stmt);

    Any evaluateExpr(Expr* expr);
    Any evaluateConst(Expr* expr);
    Any evaluateBinary(Expr* expr);
    Any evaluateUnary(Expr* expr);
    Any evaluateIdent(Expr* expr);
    Any evaluateCall(Expr* expr);
    Any evaluateArray(Expr* expr);
    Any evaluateGet(Expr* expr);
};