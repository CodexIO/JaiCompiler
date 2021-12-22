#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <malloc.h>

#include "Expr.h"
#include "error.h"

using namespace std;

int Expr::count = 0;

ostream& operator<<(ostream& out, const ET kind)
{
    const char *s = 0;
#define PROCESS_VAL(p) case (ExprType::p): s = #p; break;

    switch (kind)
    {
        PROCESS_VAL(EXPR)
        PROCESS_VAL(CONST)
        PROCESS_VAL(BINARY)
        PROCESS_VAL(UNARY)
        PROCESS_VAL(IDENT)
        PROCESS_VAL(CALL)
        PROCESS_VAL(GET)
    }
#undef PROCESS_VAL

    return out << s;
}


std::string OPtoString(OP op)
{
    const char *s = 0;
#define PROCESS_VAL(p) case (OP::p): s = #p; break;

    switch (op)
    {
        PROCESS_VAL(EQUAL)
        PROCESS_VAL(NOT_EQUAL)
        PROCESS_VAL(AND)
        PROCESS_VAL(OR)

        PROCESS_VAL(GREATER)
        PROCESS_VAL(GREATER_EQUAL)
        PROCESS_VAL(LESS)
        PROCESS_VAL(LESS_EQUAL)

        PROCESS_VAL(MINUS)
        PROCESS_VAL(PLUS)
        PROCESS_VAL(MULTIPLY)
        PROCESS_VAL(DIVIDE)

        PROCESS_VAL(NOT)
        PROCESS_VAL(NEGATE)

        PROCESS_VAL(UNKNOWN)
    }
#undef PROCESS_VAL

    return std::string(s);
}

std::ostream &operator<<(std::ostream &out, const OP op)
{
    return out << OPtoString(op);
}

std::string TypeToString(Type type)
{
    const char *s = 0;
#define PROCESS_VAL(p) case (Type::p): s = #p; break;

    switch (type)
    {
        PROCESS_VAL(NUMBER)
        PROCESS_VAL(STRING)
        PROCESS_VAL(FLOAT)
        PROCESS_VAL(DOUBLE)
        PROCESS_VAL(INT)
        PROCESS_VAL(CHAR)
        PROCESS_VAL(BOOL)

        PROCESS_VAL(S64)
        PROCESS_VAL(S32)
        PROCESS_VAL(S16)
        PROCESS_VAL(S8)
        PROCESS_VAL(U64)
        PROCESS_VAL(U32)
        PROCESS_VAL(U16)
        PROCESS_VAL(U8)

        PROCESS_VAL(VOID)
        PROCESS_VAL(ENUM)
        PROCESS_VAL(FUNCTION)

        PROCESS_VAL(UNKNOWN)
    }
#undef PROCESS_VAL
    return std::string(s);
}

Type getType(OP op) {
    switch (op) {
        case OP::NOT:
        case OP::EQUAL:
        case OP::NOT_EQUAL:
        case OP::GREATER:
        case OP::GREATER_EQUAL:
        case OP::LESS:
        case OP::LESS_EQUAL: return Type::BOOL;
        case OP::PLUS:
        case OP::MINUS:
        case OP::MULTIPLY:
        case OP::DIVIDE:  return Type::NUMBER;
        case OP::NEGATE:  return Type::NUMBER;
    }
    return Type::UNKNOWN;
}

ostream &operator<<(ostream &out, const Type type)
{
    return out << TypeToString(type);
}

ostream &operator<<(ostream &out, const Expr *expr)
{
    switch (expr->kind)
    {
    case ET::EXPR:
        return out << "Expr ";
    case ET::CONST:
        return out << (Const *)expr;
    case ET::BINARY:
        return out << (Binary *)expr;
    case ET::UNARY:
        return out << (Unary *)expr;
    case ET::IDENT:
        return out << (Ident *)expr;
    case ET::CALL:
        return out << (Call *)expr;
    case ET::GET:
        return out << (Get *)expr;
    }
    return out << "UNKNOWN Expr!";
}

Expr::Expr(Type t, TypeFlags f) : type(t, f) {
    debugCount = count++;
}

Const::Const() : Expr(Type::UNKNOWN)
{
    kind = ET::CONST;
    any.type.flags |= Flags::CONSTANT;
}
Const::Const(double d) : Const()
{
    type = Type::DOUBLE;
    any.value.Float = d;
    any.type.base = Type::FLOAT;
}
Const::Const(long long int i) : Const()
{
    type = Type::INT;
    any.value.Int = i;
    any.type.base = Type::INT;
}
Const::Const(char c) : Const()
{
    type = Type::CHAR;
    any.value.Char = c;
    any.type.base = Type::CHAR;
}
Const::Const(bool b) : Const()
{
    type = Type::BOOL;
    any.value.Bool = b;
    any.type.base = Type::BOOL;
}
Const::Const(std::string* str) : Const()
{
    type = Type::STRING;
    any.value.String = str;
    any.type.base = Type::STRING;
}
Const::Const(void *p, Type t) : Const()
{
    // CLEANUP: This doesn't do any usefull stuff atm

    type = Type::VOID;
    any.value.Ptr = p;
    any.type.base = t;
}

ostream &operator<<(ostream &out, const Const *expr)
{
    if (expr == nullptr)
        return out;

    auto v = expr->any.value;
    stringstream val;
    switch (expr->any.type.base)
    {
    case Type::STRING:
        val << "\"" << *v.String << "\"";
        break;
    case Type::DOUBLE:
    case Type::FLOAT:
        val << v.Float << "f";
        break;
    case Type::CHAR:
        val << "'" << v.Char << "'";
        break;
    case Type::BOOL:
        val << (v.Bool ? "true" : "false");
        break;

    case Type::INT:
    case Type::S64:
    case Type::S32:
    case Type::S16:
    case Type::S8:
    case Type::U64:
    case Type::U32:
    case Type::U16:
    case Type::U8:
        val << v.Int;
        break;
    case Type::ENUM:
        val << "Value = ENUM";
        break;
    case Type::FUNCTION:
        val << "Value = FUNCTION";
        break;

    case Type::VOID:
        val << "Value = VOID";
        break;
    case Type::UNKNOWN:
        val << "Value = UNKNOWN";
        break;
    }
    return out << val.str();
}

Binary::Binary(Expr *l, OP o, Expr *r) : Expr(Type::UNKNOWN), left(l), right(r), op(o) {
    type = getType(o);
    kind = ET::BINARY;
}

ostream &operator<<(ostream &out, const Binary *expr)
{
    return out << expr->left << " " << expr->op << " " << expr->right;
}

Unary::Unary(OP o, Expr *e) :  Expr(Type::UNKNOWN), expr(e), op(o) {
    type = getType(o);
    kind = ET::UNARY;
}

ostream &operator<<(ostream &out, const Unary *expr)
{
    return out << expr->op << expr->expr;
}

Ident::Ident(string n) : Expr(Type::UNKNOWN), name(n) {
    //TODO Set type acording to some table of current Identifiers
    kind = ET::IDENT;
}

ostream &operator<<(ostream &out, const Ident *expr)
{
    return out << expr->name;
}

Call::Call(Expr *d, vector<Expr *> a) :  Expr(Type::UNKNOWN), name(d), args(a) {
    //TODO Set type acording to some gable of current Functions
    kind = ET::CALL;
}

ostream &operator<<(ostream &out, const Call *expr)
{
    out << expr->name << "(";
    for (auto arg : expr->args)
        out << arg << ", ";
    return out << ")";
}

Get::Get(Expr *e, Expr* v) : Expr(Type::UNKNOWN), expr(e), access(v) {
    //TODO Set type acording to some table of current Identifiers
    kind = ET::GET;
}

ostream &operator<<(ostream &out, const Get *expr)
{
    return out << expr->expr << "." << expr->access;
}