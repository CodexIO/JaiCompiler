#pragma once

#include <string>
#include <vector>

#include "error.h"
#include "Any.h"

using namespace std;

enum class OP {
    EQUAL,
    NOT_EQUAL,
    
    AND,
    OR,

    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    
    MINUS,
    PLUS,
    MULTIPLY,
    DIVIDE,
    
    NOT,
    NEGATE,

    UNKNOWN
};

std::string OPtoString(OP op);

std::ostream& operator<<(std::ostream& out, const OP op);

ostream& operator<<(ostream& out, const Type type);

enum class ET {
    EXPR,
    CONST,
    BINARY,
    UNARY,
    IDENT,
    CALL,
    GET,
};
typedef ET ExprType;

ostream& operator<<(ostream& out, const ET kind);

struct Expr {
    static int count;

    int debugCount;
    ExprType kind = ET::EXPR;
    ImprovedType type;

    Expr(Type t, TypeFlags f = 0);
};

ostream& operator<<(ostream& out, const Expr* expr);

struct Const : Expr {
    Any any;

    Const();
    Const(double d);
    Const(long long int i);
    Const(char c);
    Const(bool b);
    Const(std::string* s);
    Const(void* p, Type t);
};

ostream& operator<<(ostream& out, const Const* expr);

struct Binary : Expr {
    Expr* left;
    Expr* right;
    OP op;

    Binary(Expr* l, OP o, Expr* r);

};

ostream& operator<<(ostream& out, const Binary* expr);

struct Unary : Expr {
    Expr* expr;
    OP op;

    Unary(OP o,Expr* e);
    
};

ostream& operator<<(ostream& out, const Unary* expr);

struct Ident : Expr {
    string name;

    Ident(string n);
    
};

ostream& operator<<(ostream& out, const Ident* expr);

struct Call : Expr {
    Expr* name;
    vector<Expr*> args;

    Call(Expr* d, vector<Expr*> a);

};

ostream& operator<<(ostream& out, const Call* expr);

struct Get : Expr {
    Expr* expr;
    Expr* access;

    Get(Expr* e, Expr* v);
};

ostream& operator<<(ostream& out, const Get* expr);
    
template <typename T>
T errorExpr(Expr* expr, const char* file, int line) {
    stringstream message;
    message << "Stmt wasn't the type we expected it to be\n";
    message << "Expected: " << typeid(T).name() << " Got: " << expr->kind << endl;
    INTERNAL_ERROR_3(message.str(), file, line);
    return nullptr;
}

#define isConst(expr)       (expr->kind == ET::CONST)
#define isBinary(expr)      (expr->kind == ET::BINARY)
#define isUnary(expr)       (expr->kind == ET::UNARY)
#define isIdent(expr)       (expr->kind == ET::IDENT)
#define isCall(expr)        (expr->kind == ET::CALL)
#define isGet(expr)         (expr->kind == ET::GET)

#define asConst(expr)       (isConst(expr)  ? (Const*)expr  : errorExpr<Const*>(expr, __FILE__, __LINE__))   
#define asBinary(expr)      (isBinary(expr) ? (Binary*)expr : errorExpr<Binary*>(expr, __FILE__, __LINE__))  
#define asUnary(expr)       (isUnary(expr)  ? (Unary*)expr  : errorExpr<Unary*>(expr, __FILE__, __LINE__))   
#define asIdent(expr)       (isIdent(expr)  ? (Ident*)expr  : errorExpr<Ident*>(expr, __FILE__, __LINE__))   
#define asCall(expr)        (isCall(expr)   ? (Call*)expr   : errorExpr<Call*>(expr, __FILE__, __LINE__))    
#define asArray(expr)       (isArray(expr)  ? (Array*)expr  : errorExpr<Array*>(expr, __FILE__, __LINE__))   
#define asGet(expr)         (isGet(expr)    ? (Get*)expr    : errorExpr<Get*>(expr, __FILE__, __LINE__)) 