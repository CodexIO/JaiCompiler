#include <string>
#include <vector>
#include <sstream>
#include <tuple>

#include "Stmt.h"
#include "Token.h"
#include "Expr.h"

using namespace std;
using namespace Flags;

ostream& operator<<(ostream& out, const StmtType kind)
{
    const char *s = 0;
#define PROCESS_VAL(p) case (StmtType::p): s = #p; break;

    switch (kind)
    {
        PROCESS_VAL(STMT)
        PROCESS_VAL(DECL)
        PROCESS_VAL(BLOCK)
        PROCESS_VAL(STRUCT)
        PROCESS_VAL(ENUM)
        PROCESS_VAL(FUNC)
        PROCESS_VAL(RETURN)
        PROCESS_VAL(IF)
        PROCESS_VAL(EXPRSTMT)
        PROCESS_VAL(DEFER)
        PROCESS_VAL(FOR)
        PROCESS_VAL(WHILE)
        PROCESS_VAL(ASSIGN)
        PROCESS_VAL(SET)
        PROCESS_VAL(CONTINUE)
        PROCESS_VAL(BREAK)
    }
#undef PROCESS_VAL

    return out << s;
}

ostream &operator<<(ostream &out, const Stmt *stmt)
{
    switch (stmt->kind)
    {
    case ST::STMT:
        return out << "Stmt: ";
    case ST::DECL:
        return out << (Decl *)stmt;
    case ST::BLOCK:
        return out << (Block *)stmt;
    case ST::STRUCT:
        return out << (Struct *)stmt;
    case ST::ENUM:
        return out << (Enum *)stmt;
    case ST::FUNC:
        return out << (Func *)stmt;
    case ST::RETURN:
        return out << (Return *)stmt;
    case ST::IF:
        return out << (If *)stmt;
    case ST::EXPRSTMT:
        return out << (ExprStmt *)stmt;
    case ST::DEFER:
        return out << (Defer *)stmt;
    case ST::FOR:
        return out << (For *)stmt;
    case ST::WHILE:
        return out << (While *)stmt;
    case ST::ASSIGN:
        return out << (Assign *)stmt;
    case ST::SET:
        return out << (Set *)stmt;
    case ST::CONTINUE:
        return out << (Continue *)stmt;
    case ST::BREAK:
        return out << (Break *)stmt;
    }
    return out << "UNKNOWN STMT: ";
}


/*
Decl::Decl(string n, Type t, TypeFlags f) : name(n), type(t, f) {
    kind = ST::DECL;

    switch(t) {
        case(Type::STRING): {
            expr = new Const(new string(""));
        } break;
        case (Type::FLOAT):
        case (Type::DOUBLE): {
            expr = new Const(0.);
        } break;
        case (Type::CHAR): {
            expr = new Const('\0');
        } break;
        case (Type::BOOL): {
            expr = new Const(false);
        } break;
        default: {
            long long int i = 0;
            expr = new Const(i);
        }
    }
}*/

Decl::Decl(string n, ImprovedType t, Expr* e) : name(n), type(t), expr(e)
{
    kind = ST::DECL;
}

bool Decl::isConstant()
{
    return this->type.flags & CONSTANT;
}

bool Decl::isPointer()
{
    return this->type.flags & POINTER;
}

bool Decl::isDeclArray()
{
    return this->type.flags & ARRAY;
}

ostream &operator<<(ostream &out, const Decl *decl)
{
    bool isPointer = decl->type.flags & POINTER;
    bool isArray = decl->type.flags & ARRAY;
    bool isConst = decl->type.flags & CONSTANT;
    stringstream type;
    type << (isConst ? "const " : "") << (isPointer ? "*" : "") << (isArray ? "[]" : "");
    stringstream value;
    if (decl->expr)
        value << " = " << decl->expr;
    return out << "Decl: " << decl->name << " is " << type.str() << decl->type.base << value.str();
}

Block::Block(vector<Stmt *> s) : stmts(s)
{
    kind = ST::BLOCK;
}

ostream &operator<<(ostream &out, const Block *block)
{
    out << " {\n";
    for (Stmt *stmt : block->stmts)
    {
        out << stmt << "\n";
    }
    out << "}";
    return out;
}

Struct::Struct(string n, Block *b) : name(n), body(b)
{
    kind = ST::STRUCT;

    int i = 0;
    for (auto stmt : body->stmts) {
        //TODO: also add functions of the Struct to this

        if (isDecl(stmt)) {
            Decl* decl = asDecl(stmt);
            if (! (decl->type.flags & Flags::CONSTANT) ) {
                memberPositions[decl->name] = i++;
                memberTypes.push_back(decl->type);
            }
        }
    }
}

ostream &operator<<(ostream &out, const Struct *stmt)
{
    return out << stmt->name << " :: Struct" << stmt->body << endl;
}

Enum::Enum(string n, vector<string> names) : name(n)
{
    for (int i = 0; i < names.size(); i++)
    {
        string s = names[i];
        values.push_back(make_tuple(s, i));
    }
    kind = ST::ENUM;
}

ostream &operator<<(ostream &out, const Enum *stmt)
{
    stringstream names;
    for (auto tuple : stmt->values)
        names << get<0>(tuple) << ", ";
    return out << stmt->name << " :: Enum {\n"
               << names.str() << "\n}" << endl;
}

Func::Func(string n, vector<Decl *> p, ImprovedType t, Block *b)
    : name(n), params(p), returnType(t), body(b)
{
    kind = ST::FUNC;
}

ostream &operator<<(ostream &out, const Func *func)
{
    out << func->name << " :: (";
    for (auto param : func->params)
        out << param << ", ";

    return out << ") -> " << func->returnType.base << func->body << endl;
}

Return::Return(Expr *e) : expr(e)
{
    kind = ST::RETURN;
}

ostream &operator<<(ostream &out, const Return *stmt)
{
    return out << "return " << stmt->expr;
}

If::If(Expr *c, Stmt* ifB, Stmt* elseB) : condition(c), ifBody(ifB), elseBody(elseB) 
{
    kind = ST::IF;
}

ostream &operator<<(ostream &out, const If *stmt)
{
    out << "if " << stmt->condition << " " << stmt->ifBody;
    if (stmt->elseBody) out << " else " << stmt->elseBody;
    return out;
}

ExprStmt::ExprStmt(Expr *e) : expr(e)
{
    kind = ST::EXPRSTMT;
}

ostream &operator<<(ostream &out, const ExprStmt *stmt)
{
    return out << stmt->expr;
}

Defer::Defer(Stmt *b) : block(b)
{
    kind = ST::DEFER;
}

ostream &operator<<(ostream &out, const Defer *stmt)
{
    return out << "defer " << stmt->block << endl;
}

For::For(string i, Expr *s, Expr *e, Stmt *b) : it(i), start(s), end(e), body(b)
{
    kind = ST::FOR;
}
For::For(string i, Expr *array, Stmt *b) : it(i), start(array), body(b)
{
    kind = ST::FOR;
}

ostream &operator<<(ostream &out, const For *stmt)
{
    return out << "for " << stmt->it << ": " << stmt->start << stmt->body << endl;
}

While::While(Expr *c, Stmt *b) : condition(c), body(b)
{
    kind = ST::WHILE;
}

ostream &operator<<(ostream &out, const While *stmt)
{
    return out << "while " << stmt->condition << stmt->body << endl;
}

Assign::Assign(Expr *l, Expr *r) : left(l), right(r)
{
    kind = ST::ASSIGN;
}

ostream &operator<<(ostream &out, const Assign *stmt)
{
    return out << stmt->left << " = " << stmt->right;
}

Set::Set(Expr *e, Expr* a, Expr *v) : expr(e), access(a), value(v) {
    kind = ST::SET;
}

ostream &operator<<(ostream &out, const Set *stmt)
{
    return out << stmt->expr << "." << stmt->access << " = " << stmt->value;
}

Continue::Continue()
{
    kind = ST::CONTINUE;
}

ostream &operator<<(ostream &out, const Continue *stmt)
{
    return out << "continue ";
}

Break::Break()
{
    kind = ST::BREAK;
}

ostream &operator<<(ostream &out, const Break *stmt)
{
    return out << "break ";
}