#include <vector>
#include <string>
#include <sstream>

#include "Parser.h"
#include "Lexer.h"
#include "Token.h"
#include "error.h"
#include "Stmt.h"
#include "Expr.h"

using namespace std;
using namespace Flags;

Parser::Parser(const char *filePath) : lx(filePath) {}

OP Parser::toOperand(Token tk)
{
    switch (tk.type)
    {
    case BANG:
        return OP::NOT;
    case EQUAL_EQUAL:
        return OP::EQUAL;
    case AND:
        return OP::AND;
    case OR:
        return OP::OR;
    case BANG_EQUAL:
        return OP::NOT_EQUAL;
    case LESS:
        return OP::LESS;
    case LESS_EQUAL:
        return OP::LESS_EQUAL;
    case GREATER:
        return OP::GREATER;
    case GREATER_EQUAL:
        return OP::GREATER_EQUAL;
    case PLUS:
        return OP::PLUS;
    case MINUS:
        return OP::MINUS;
    case STAR:
        return OP::MULTIPLY;
    case SLASH:
        return OP::DIVIDE;
    default:
        return OP::UNKNOWN;
    }
}

string* Parser::toString(Token &tk)
{
    string* value = new string(tk.source, 1, tk.source.length() - 2);
    return value;
}

bool Parser::toBool(Token &tk)
{
    return tk.source == "TRUE";
}

char Parser::toChar(Token &tk)
{
    ASSERT(tk.type == STRING_CONSTANT);
    ASSERT(tk.source.size() == 3);
    return tk.source[1];
}

long long int Parser::toInt(Token &tk)
{
    string s = tk.source;
    int index = 0;

    bool negate = (s[0] == '-');
    if (s[0] == '-')
        index++;

    int result = 0;
    for (int i = index; i < s.length(); i++)
    {
        char c = s[i];
        if (c < '0' || c > '9')
            error("Numbers can only contain digits.");

        result = result * 10 + (c - '0');
    }
    return negate ? -result : result;
}

double Parser::toDouble(Token &tk)
{
    string s = tk.source;
    int index = 0;
    bool pointFound = false;

    bool negate = (s[0] == '-');
    if (s[0] == '-')
        index++;

    double beforePoint = 0;
    double afterPoint = 0;
    for (int i = index; i < s.length(); i++)
    {
        char c = s[i];
        if (c == '.')
        {
            pointFound = true;
            continue;
        }
        if (c < '0' || c > '9')
            error("Numbers can only contain digits.");

        if (pointFound)
            afterPoint = afterPoint * 10 + (c - '0');
        else
            beforePoint = beforePoint * 10 + (c - '0');
    }

    while (afterPoint > 0)
        afterPoint = afterPoint / 10;

    double result = beforePoint + afterPoint;

    return negate ? -result : result;
}

Token Parser::nextToken()
{
    prevTk = tk;
    tk = lx.nextToken();
    return tk;
}

bool Parser::match(TkType type)
{
    if (tk.type == type)
    {
        nextToken();
        return true;
    }
    return false;
}

Type Parser::getType(string &s)
{
    if (s == "void")
        return Type::VOID;
    if (s == "float")
        return Type::FLOAT;
    if (s == "double")
        return Type::DOUBLE;
    if (s == "int")
        return Type::INT;
    if (s == "bool")
        return Type::BOOL;
    if (s == "string")
        return Type::STRING;
    if (s == "s64")
        return Type::S64;
    if (s == "s32")
        return Type::S32;
    if (s == "s16")
        return Type::S16;
    if (s == "s8")
        return Type::S8;
    if (s == "u64")
        return Type::U64;
    if (s == "u32")
        return Type::U32;
    if (s == "u16")
        return Type::U16;
    if (s == "u8")
        return Type::U8;

    return Type::UNKNOWN;
}

Expr *Parser::primary()
{
    switch (tk.type)
    {
    case NUMBER_CONSTANT:   nextToken(); return new Const(toInt(prevTk));
    case DOUBLE_CONSTANT:   nextToken(); return new Const(toDouble(prevTk));
    case STRING_CONSTANT:   nextToken(); return new Const(toString(prevTk));
    case BOOL_CONSTANT:     nextToken(); return new Const(toBool(prevTk));
    case NULL_CONSTANT:     nextToken(); return new Const(nullptr);
    case IDENTIFIER:        nextToken(); return new Ident(prevTk.source);
    case CHAR_CONSTANT:     nextToken(); nextToken(); return new Const(toChar(prevTk));
    case OPEN_PAREN: {
        nextToken();
        Expr *expr = parseExpression();
        CONSUME(CLOSE_PAREN);
        return expr;
    }
    default:
    {
        vector<string> expected{"NUMBER", "STRING", "BOOL", "NULL", "("};
        ERROR(prevTk.source, expected, tk);
    }
    }
    return nullptr;
}

Expr *Parser::call()
{
    Expr *expr = primary();

    while (true)
    {
        if (match(OPEN_PAREN))
        {

            expr = parseArguments(expr);
        }
        else if (match(POINT))
        {

            CONSUME(IDENTIFIER);
            Expr* name = new Const(new string(prevTk.source));
            expr = new Get(expr, name);
        }
        else if (match(OPEN_BRACKET))
        {

            Expr *index = parseExpression();
            expr = new Get(expr, index);

            CONSUME(CLOSE_BRACKET);
        }
        else
            break;
    }

    return expr;
}

Expr *Parser::parseArguments(Expr *callee)
{
    vector<Expr *> args;

    if (tk.type != CLOSE_PAREN)
        args.push_back(parseExpression());

    while (match(COMMA))
        args.push_back(parseExpression());

    CONSUME(CLOSE_PAREN);

    return new Call(callee, args);
}

Expr *Parser::unary()
{
    if (match(BANG, MINUS))
    {
        Token op = prevTk;
        Expr *right = unary();
        return new Unary(toOperand(op), right);
    }

    return call();
}

Expr *Parser::factor()
{
    Expr *expr = unary();

    while (match(SLASH, STAR))
    {
        Token op = prevTk;
        Expr *right = unary();
        expr = new Binary(expr, toOperand(op), right);
    }
    return expr;
}

Expr *Parser::term()
{
    Expr *expr = factor();

    while (match(MINUS, PLUS))
    {
        Token op = prevTk;
        Expr *right = factor();
        expr = new Binary(expr, toOperand(op), right);
    }
    return expr;
}

Expr *Parser::comparison()
{
    Expr *expr = term();

    while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL))
    {
        Token op = prevTk;
        Expr *right = comparison();
        expr = new Binary(expr, toOperand(op), right);
    }
    return expr;
}

Expr *Parser::equality()
{
    Expr *expr = comparison();

    while (match(BANG_EQUAL, EQUAL_EQUAL))
    {
        Token op = prevTk;
        Expr *right = comparison();
        expr = new Binary(expr, toOperand(op), right);
    }
    return expr;
}

Expr *Parser::logicAnd()
{
    Expr *expr = equality();

    while (match(AND))
    {
        Token op = prevTk;
        Expr *right = equality();
        expr = new Binary(expr, toOperand(op), right);
    }
    return expr;
}

Expr *Parser::logicOr()
{
    Expr *expr = logicAnd();

    while (match(OR))
    {
        Token op = prevTk;
        Expr *right = logicAnd();
        expr = new Binary(expr, toOperand(op), right);
    }
    return expr;
}

Expr *Parser::parseExpression()
{
    Expr *expr = logicOr();

    return expr;
}

Stmt *Parser::parseFunctionDefinition(string &name)
{
    vector<Decl *> params;
    Block *body;
    ImprovedType returnType(Type::VOID, 0);

    CONSUME(OPEN_PAREN);

    if (tk.type != CLOSE_PAREN)
        params.push_back(parseFunctionParameter());

    while (tk.type != CLOSE_PAREN)
    {
        CONSUME(COMMA);
        Decl *param = parseFunctionParameter();
        params.push_back(param);
    }

    CONSUME(CLOSE_PAREN);

    if (match(ARROW))
    {
        // TODO: Make this accept other types then just normal ones, like pointers
        if (match(TYPE)) returnType.base = getType(prevTk.source);
        
        if (match(IDENTIFIER)) returnType.base = Type::TO_INFER;
    }

    body = parseBlock();

    return new Func(name, params, returnType, body);
}

Block *Parser::parseBlock()
{
    CONSUME(OPEN_CURLY);
    vector<Stmt *> stmts;

    while (tk.type != CLOSE_CURLY)
    {
        Stmt *stmt = parseStatement();
        stmts.push_back(stmt);
    }
    
    CONSUME(CLOSE_CURLY);

    return new Block(stmts);
}

Decl *Parser::parseFunctionParameter()
{
    CONSUME(IDENTIFIER);
    string name = prevTk.source;
    ImprovedType type(Type::UNKNOWN);
    Expr* expr = nullptr;

    CONSUME(COLON);

    if (match(TYPE)) type.base = getType(prevTk.source);

    else if (match(IDENTIFIER)) {
        type.info = (void*) new string(prevTk.source);
        type.base = Type::TO_INFER;
    }

    if (match(EQUAL)) expr = parseExpression();   

    return new Decl(name, type, expr);
}

Stmt *Parser::parseStruct(string &name)
{

    Block* block = parseBlock();
    /*vector<Stmt *> stmts;
    while (tk.type != CLOSE_CURLY)
    {
        Stmt *stmt = parseIdentifierStatement();
        stmts.push_back(stmt);
        CONSUME(SEMICOLON);
    }
    block = new Block(stmts);
    CHECK(CLOSE_CURLY);*/
    return new Struct(name, block);
}

Stmt *Parser::parseEnum(string name)
{
    CONSUME(OPEN_CURLY);
    vector<string> names;

    CONSUME(IDENTIFIER);
    names.push_back(prevTk.source);

    while (tk.type != CLOSE_CURLY)
    {
        CONSUME(COMMA);
        names.push_back(tk.source);
        CONSUME(IDENTIFIER);
    }

    CONSUME(CLOSE_CURLY);

    return new Enum(name, names);
}

Stmt *Parser::parseWhileStatement()
{
    CONSUME(WHILE);

    Expr *condition = parseExpression();

    Stmt *block = parseStatement();

    return new While(condition, block);
}

Stmt *Parser::parseForStatement()
{
    CONSUME(FOR);
    string it = "it";
    Expr *startOrArray;
    Expr *end = nullptr;

    // TODO: Support default iterator name
    if (match(IDENTIFIER))
    {
        it = prevTk.source;

        CONSUME(COLON);
    }

    startOrArray = parseExpression();

    if (match(POINT_POINT))
        end = parseExpression();

    Stmt *block = parseStatement();

    if (!end)
        return new For(it, startOrArray, block);
    else
        return new For(it, startOrArray, end, block);
}

Stmt *Parser::parseDeferStatement()
{
    CONSUME(DEFER);

    Stmt *block = parseStatement();

    return new Defer(block);
}

Stmt *Parser::parseIfStatement()
{
    CONSUME(IF);

    Expr* condition = parseExpression();

    match(THEN);

    Stmt* ifBody = parseStatement();

    Stmt* elseBody = nullptr;
    if (match(ELSE)) elseBody = parseStatement();

    return new If(condition, ifBody, elseBody);
}

Stmt *Parser::parseReturnStatement()
{
    CONSUME(RETURN);

    Expr *expr = parseExpression();

    CONSUME(SEMICOLON);

    return new Return(expr);
}

Stmt *Parser::parseIdentifierStatement()
{
    string name = tk.source;
    ImprovedType type(Type::UNKNOWN);
    nextToken();
    if (match(COLON))
    {
        if (tk.type == TYPE || tk.type == IDENTIFIER || tk.type == STAR)
        {

            if (match(STAR)) type.flags |= POINTER;


            if (match(IDENTIFIER))
            {
                type = Type::TO_INFER;
                type.info = (void*) new string(prevTk.source);
            }
            else if (match(TYPE))
            {
                type = getType(prevTk.source);
            }


            if (match(OPEN_BRACKET))
            {
                type.flags |= ARRAY;

                Expr *expr = parseExpression();
                CONSUME(CLOSE_BRACKET);
                CONSUME(SEMICOLON);
                return new Decl(name, type, expr);
            }

            if (match(SEMICOLON)) return new Decl(name, type, nullptr);
        }

        CONSUME(EQUAL);
        Expr *expr = parseExpression();
        CONSUME(SEMICOLON);
        return new Decl(name, type, expr);
    }
    else if (match(EQUAL))
    {

        Ident* ident = new Ident(name);
        Expr* expr = parseExpression();
        CONSUME(SEMICOLON);
        return new Assign(ident, expr);

    }
    else if (match(COLON_COLON))
    {
        type.flags |= CONSTANT;

        if (match(BOOL_CONSTANT))
        {
            type.base = Type::BOOL;
            CONSUME(SEMICOLON);
            if (prevTk.source == "false")
                return new Decl(name, type, new Const(false));
            if (prevTk.source == "true")
                return new Decl(name, type, new Const(true));
        }
        else if (match(STRING_CONSTANT))
        {

            type.base = Type::STRING;
            Expr *expr = new Const(toString(prevTk));
            CONSUME(SEMICOLON);
            return new Decl(name, type, expr);

        }
        else if (match(NUMBER_CONSTANT))
        {

            type.base = Type::S64;
            Expr *expr = new Const(toInt(prevTk));
            CONSUME(SEMICOLON);
            return new Decl(name, type, expr);
        
        }
        else if (match(DOUBLE_CONSTANT))
        {

            type.base = Type::DOUBLE;
            Expr *expr = new Const(toDouble(prevTk));
            CONSUME(SEMICOLON);
            return new Decl(name, type, expr);
        
        }
        else if (tk.type == OPEN_PAREN)
        {
            return parseFunctionDefinition(name);
        }
        else if (match(IDENTIFIER))
        {
            Expr* left  = new Ident(name); 
            Expr* right = new Ident(prevTk.source);
            CONSUME(SEMICOLON);
            return new Assign(left, right);
        }
        else if (match(TkType::STRUCT))
        {
            return parseStruct(name);
        }
        else if (match(TkType::ENUM))
        {
            return parseEnum(name);
        }
        else
        {
            vector<string> expected{"Constant", "struct", "enum", "("};
            ERROR(prevTk.source, expected, tk);
        }
    }
    else if (match(OPEN_PAREN))
    {

        Expr *callee = new Ident(name);
        auto args = parseArguments(callee);

        CONSUME(SEMICOLON);
        return new ExprStmt(args);
    }
    else if (match(OPEN_BRACKET))
    {
        //TODO: Support multiple Gets after one another

        Expr *left = new Ident(name);

        Expr *access = parseExpression();

        CONSUME(CLOSE_BRACKET);
        CONSUME(EQUAL);

        Expr *right = parseExpression();

        CONSUME(SEMICOLON);
        return new Set(left, access, right);
    }
    else if (match(POINT))
    {
        //TODO: Support mutliple gets and not only one.
        
        Expr *expr = new Ident(name);
        CONSUME(IDENTIFIER);
        
        Expr* access = new Const(new string(prevTk.source));

        CONSUME(EQUAL);

        Expr *value = parseExpression();

        CONSUME(SEMICOLON);
        return new Set(expr, access, value);
    }
    else
    {
        vector<string> expected{"::", ":"};
        ERROR(prevTk.source, expected, tk);
    }

    return new Stmt();
}

Stmt *Parser::parseStatement()
{
    switch (tk.type)
    {
    case IDENTIFIER:
        return parseIdentifierStatement();
    case RETURN:
        return parseReturnStatement();
    case IF:
        return parseIfStatement();
    case DEFER:
        return parseDeferStatement();
    case FOR:
        return parseForStatement();
    case WHILE:
        return parseWhileStatement();
    case OPEN_CURLY:
        return parseBlock();
    case CONTINUE:
        CONSUME(CONTINUE);
        CONSUME(SEMICOLON);
        return new Continue();
    case BREAK:
        CONSUME(BREAK);
        CONSUME(SEMICOLON);
        return new Break();
    default:
    {
        stringstream message;
        message << "Expceted Keyword or Identifer, but got Type: " << tk.type << " with: " << tk.source;
        error(message.str(), tk);
    }
    }
    return new Stmt();
}

void Parser::parse()
{
    nextToken();

    while (tk.type != END)
    {
        if (match(LOAD))
        {
            CONSUME(STRING_CONSTANT);
            string* file = toString(prevTk);
            loadFile(*file);
            CONSUME(SEMICOLON);
            continue;
        }

        Stmt *st = parseStatement();
        statements.push_back(st);

        // TODO: CLEANUP: These are not all the Declarations in the programm.
        if (st->kind == ST::DECL) declarations.push_back(asDecl(st));
    }
}

string Parser::loadFile(string file)
{
    return "TROLOLOLOL";
}
