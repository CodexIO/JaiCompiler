#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <tuple>
#include <unordered_map>

#include "Token.h"
#include "Expr.h"
#include "error.h"
#include "Any.h"

using namespace std;

enum class ST {
    STMT,
    DECL,
    BLOCK,
    STRUCT,
    ENUM,
    FUNC,
    RETURN,
    IF,
    EXPRSTMT,
    DEFER,
    FOR,
    WHILE,
    ASSIGN,
    SET,
    CONTINUE,
    BREAK
};
typedef ST StmtType;

ostream& operator<<(ostream& out, const StmtType kind);

struct Stmt {
    StmtType kind = ST::STMT;
    Token tk; // For Debug purposes
};

ostream& operator<<(ostream& out, const Stmt* stmt);

struct Decl : Stmt {
    string name;
    ImprovedType type;
    Expr* expr;

    //Decl(string n, Type t, TypeFlags f, Expr* e);
    //Decl(string n, Type t, TypeFlags f);
    Decl(string n, ImprovedType t, Expr* e);

    bool isConstant();
    bool isPointer();
    
    // Other name because we have a isArray Makro in Expr TODO: merge Array into Get
    bool isDeclArray(); 
};

ostream& operator<<(ostream& out, const Decl* stmt);

struct Block : Stmt {
    vector<Stmt*> stmts;

    Block(vector<Stmt*> s);
};

ostream& operator<<(ostream& out, const Block* block);

struct Struct : Stmt {
    string name;
    Block* body;

    // The Position in the vector represents where the value will be saved in MyStruct
    unordered_map<string, int> memberPositions;
    vector<ImprovedType> memberTypes;
    Struct(string n, Block* b);
};

ostream& operator<<(ostream& out, const Struct* stmt);

struct Enum : Stmt {
    string name;
    vector<tuple<string, int>> values;

    Enum(string n, vector<string> names);
};

ostream& operator<<(ostream& out, const Enum* stmt);

struct Func : Stmt {
    string name;
    vector<Decl*> params;
    Block* body;
    ImprovedType returnType;

    Func(string n, vector<Decl*> p, ImprovedType t, Block* b);
};

ostream& operator<<(ostream& out, const Func* func);

struct Return : Stmt {
    Expr* expr;

    Return (Expr* e);
};

ostream& operator<<(ostream& out, const Return* stmt);

struct If : Stmt {
    // TODO If should include the Else Stmt
    Expr* condition;
    Stmt* ifBody;
    Stmt* elseBody;

    If(Expr* c, Stmt* ifB, Stmt* elseB);
};

ostream& operator<<(ostream& out, const If* stmt);

struct ExprStmt : Stmt {
    Expr* expr;

    ExprStmt(Expr* e);
};

ostream& operator<<(ostream& out, const ExprStmt* stmt);

struct Defer : Stmt {
    Stmt* block;

    Defer(Stmt* b);
};

ostream& operator<<(ostream& out, const Defer* stmt);

struct For : Stmt {
    string it;
    Expr* start;
    Expr* end;
    Stmt* body;

    For(string i, Expr* s, Expr* e, Stmt* b);
    For(string i, Expr* array, Stmt* b);
};

ostream& operator<<(ostream& out, const For* stmt);

struct While : Stmt {
    Expr* condition;
    Stmt* body;

    While(Expr* c, Stmt* b);
};

ostream& operator<<(ostream& out, const While* stmt);

struct Assign : Stmt {
    Expr* left;
    Expr* right;

    Assign(Expr* l, Expr* r);
};

ostream& operator<<(ostream& out, const Assign* stmt);

struct Set : Stmt {
    Expr* expr;
    Expr* access;
    Expr* value;

    Set(Expr* e, Expr* access, Expr* v);
};

ostream& operator<<(ostream& out, const Set* stmt);

struct Continue : Stmt {

    Continue();
};

ostream& operator<<(ostream& out, const Continue* stmt);

struct Break : Stmt {

    Break();
};

ostream& operator<<(ostream& out, const Break* stmt);

template <typename T>
T errorStmt(Stmt* stmt, const char* file, int line) {
    stringstream message;
    message << "Stmt wasn't the type we expected it to be\n";
    message << "Expected: " << typeid(T).name() << " Got: " << stmt->kind << endl;
    INTERNAL_ERROR_3(message.str(), file, line);
    return nullptr;
}

#define isDecl(stmt)        stmt->kind == ST::DECL
#define isBlock(stmt)       stmt->kind == ST::BLOCK
#define isStruct(stmt)      stmt->kind == ST::STRUCT
#define isEnum(stmt)        stmt->kind == ST::ENUM
#define isFunc(stmt)        stmt->kind == ST::FUNC
#define isReturn(stmt)      stmt->kind == ST::RETURN
#define isIf(stmt)          stmt->kind == ST::IF
#define isExprStmt(stmt)    stmt->kind == ST::EXPRSTMT
#define isDefer(stmt)       stmt->kind == ST::DEFER
#define isFor(stmt)         stmt->kind == ST::FOR
#define isWhile(stmt)       stmt->kind == ST::WHILE
#define isAssign(stmt)      stmt->kind == ST::ASSIGN
#define isSet(stmt)         stmt->kind == ST::SET
#define isContinue(stmt)    stmt->kind == ST::CONTINUE
#define isBreak(stmt)       stmt->kind == ST::BREAK

#define asDecl(stmt)        (isDecl(stmt)     ? (Decl*)stmt    : errorStmt<Decl*>(stmt, __FILE__, __LINE__))
#define asBlock(stmt)       (isBlock(stmt)    ? (Block*)stmt   : errorStmt<Block*>(stmt, __FILE__, __LINE__))
#define asStruct(stmt)      (isStruct(stmt)   ? (Struct*)stmt  : errorStmt<Struct*>(stmt, __FILE__, __LINE__))
#define asEnum(stmt)        (isEnum(stmt)     ? (Enum*)stmt    : errorStmt<Enum*>(stmt, __FILE__, __LINE__))
#define asFunc(stmt)        (isFunc(stmt)     ? (Func*)stmt    : errorStmt<Func*>(stmt, __FILE__, __LINE__))
#define asReturn(stmt)      (isReturn(stmt)   ? (Return*)stmt  : errorStmt<Return*>(stmt, __FILE__, __LINE__))
#define asIf(stmt)          (isIf(stmt)       ? (If*)stmt      : errorStmt<If*>(stmt, __FILE__, __LINE__))
#define asElse(stmt)        (isElse(stmt)     ? (Else*)stmt    : errorStmt<Else*>(stmt, __FILE__, __LINE__))
#define asExprStmt(stmt)    (isExprStmt(stmt) ? (ExprStmt*)stmt: errorStmt<ExprStmt*>(stmt, __FILE__, __LINE__))
#define asDefer(stmt)       (isDefer(stmt)    ? (Defer*)stmt   : errorStmt<Defer*>(stmt, __FILE__, __LINE__))
#define asFor(stmt)         (isFor(stmt)      ? (For*)stmt     : errorStmt<For*>(stmt, __FILE__, __LINE__))
#define asWhile(stmt)       (isWhile(stmt)    ? (While*)stmt   : errorStmt<While*>(stmt, __FILE__, __LINE__))
#define asAssign(stmt)      (isAssign(stmt)   ? (Assign*)stmt  : errorStmt<Assign*>(stmt, __FILE__, __LINE__))
#define asSet(stmt)         (isSet(stmt)      ? (Set*)stmt     : errorStmt<Set*>(stmt, __FILE__, __LINE__))
#define asContinue(stmt)    (isContinue(stmt) ? (Continue*)stmt: errorStmt<Continue*>(stmt, __FILE__, __LINE__))
#define asBreak(stmt)       (isBreak(stmt)    ? (Break*)stmt   : errorStmt<Break*>(stmt, __FILE__, __LINE__))