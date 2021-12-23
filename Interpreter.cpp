#include <vector>
#include <string>
#include <iostream>

#include "Interpreter.h"
#include "Stmt.h"
#include "Expr.h"
#include "Parser.h"
#include "error.h"

using std::cout, std::endl, std::string, std::vector;

Interpreter::Interpreter(Parser &p) : parser(p) {
    ImprovedType type(Type::STRING, 0);
    vector<Decl*> params = {new Decl("text", type, new Const("Hello World"))};

    const char* name = "printf";
    Func* printf = new Func(name, params, Type::VOID, nullptr);

    functions[name] = printf;
}

void Interpreter::run()
{
    cout << "Running: " << endl;

    setUpTables(new Block(parser.statements));

    inferTypes(parser.declarations);

    callFunction(main);
}

void Interpreter::inferTypes(vector<Decl*> decls)
{
    for (auto decl : decls) {
        if (decl->type.base == Type::TO_INFER) {
            string* name = (string*) decl->type.info;
            
            if (structs.contains(*name)) decl->type.base = Type::STRUCT;
            else if (enums.contains(*name)) decl->type.base = Type::ENUM;

        }
    }
}

void Interpreter::setUpTables(Block* block)
{
    for (Stmt *stmt : block->stmts)
    {
        switch(stmt->kind) {
            case (ST::STMT): INTERNAL_ERROR("We shouldn't have wild normal Stmts running around but here we are.");
            case (ST::STRUCT): {
                Struct* s = asStruct(stmt); structs[s->name] = s; 
            } break;
            case (ST::ENUM): {
                Enum* e = asEnum(stmt); enums[e->name] = e;
            } break;
            case (ST::FUNC): {
                Func* f = asFunc(stmt);
                functions[f->name] = f;
                if (f->name == "main") main = f;
                setUpTables(f->body);
            } break;
            case (ST::DECL): {
                Decl* decl = asDecl(stmt);
                if (decl->type.flags & Flags::CONSTANT) {
                    Const* c = asConst(decl->expr);
                    constants[decl->name] = c->any;
                }
            } break;
            case (ST::BLOCK): {
                Block* b = asBlock(stmt);
                setUpTables(b);
            } break;
            case (ST::FOR): {
                For* forLoop = asFor(stmt);
                setUpTables(forLoop->body);
            } break;
            case (ST::WHILE): {
                While* whileLoop = asWhile(stmt);
                setUpTables(whileLoop->body);
            } break;
            case (ST::IF): {
                If* i = asIf(stmt);
                setUpTables(i->ifBody);
                setUpTables(i->elseBody);
            } break;
        }
    }
}

void Interpreter::setUpTables(Stmt* stmt)
{    
    if (!stmt) return;

    if (isBlock(stmt)) setUpTables(asBlock(stmt));
}

void Interpreter::callPrintf(Func* func)
{
    Any any = variables[func->params[0]->name];

    ASSERT(any.type.base == Type::STRING);

    string text = *any.value.String;

    cout << "> " << text << endl;
}

Any Interpreter::callFunction(Func* func)
{
    if (func->name == "printf") {
        callPrintf(func);
        Any any;
        return any;
    }
 
    pushDefers();

    runBlock(func->body);

    executeDefers();

    shouldReturn = false;

    return returnValue;
}

void Interpreter::pushDefers()
{
    vector<Stmt*> vec = {};
    deferStatements.push(vec);
}

void Interpreter::executeDefers()
{
    if (deferStatements.empty()) return;

    auto stmts = deferStatements.top();
    for (auto stmt : stmts) {
        runStmt(stmt);
    }

    deferStatements.pop();
}

void Interpreter::runStmt(Stmt* stmt)
{
    switch(stmt->kind) {
        case (ST::STMT):    INTERNAL_ERROR("We shouldn't have wild normal Stmts running around but here we are.");
        case (ST::DECL):    runDecl(stmt);      break;
        case (ST::BLOCK):   runBlock(stmt);     break;
        case (ST::STRUCT):  runStruct(stmt);    break;
        case (ST::ENUM):    runEnum(stmt);      break;
        case (ST::FUNC):    runFunc(stmt);      break;
        case (ST::RETURN):  runReturn(stmt);    break;
        case (ST::IF):      runIf(stmt);        break;
        case (ST::EXPRSTMT):runExprStmt(stmt);  break;
        case (ST::DEFER):   runDefer(stmt);     break;
        case (ST::FOR):     runFor(stmt);       break;
        case (ST::WHILE):   runWhile(stmt);     break;
        case (ST::ASSIGN):  runAssign(stmt);    break;
        case (ST::SET):     runSet(stmt);       break;
        case (ST::CONTINUE):runContinue(stmt);  break;
        case (ST::BREAK):   runBreak(stmt);     break;
    }
}

void Interpreter::runDecl(Stmt* stmt)
{
    Decl* decl = asDecl(stmt);

    if (decl->isConstant()) return;


    Any any;
    any.type = decl->type;

    // Decl of Array
    if (decl->type.flags & Flags::ARRAY)
    {
        ImprovedType t(decl->type.base, 0);
        any.value.Ptr = new MyArray(t, evaluateExpr(decl->expr).value.Int);
    }
    else if (decl->type.base == Type::STRUCT)
    {

        Struct* defn  = (Struct*) decl->type.info;
        any.value.Ptr = new MyStruct(defn);

    }
    else if (decl->type.base == Type::ENUM)
    {

        Any tmp = evaluateExpr(decl->expr);

        string* enumName = tmp.value.String;

        if (!enums.contains(*enumName)) error("Struct not defined");

        // CLEANUP: I think we should have a special type for enum. But for now we don't
        any.value.Int = 0;
        
    }
    else if (decl->expr) any = evaluateExpr(decl->expr);
    
    variables[decl->name] = any;
}

void Interpreter::runBlock(Stmt* stmt)
{
    pushDefers();
    Block* block = asBlock(stmt);

    for (auto s : block->stmts) {
        //cout << "Executing: " << s << endl;
        runStmt(s);
        if (shouldReturn || shouldContinue || shouldBreak) break;
    }

    shouldContinue = false;
    executeDefers();
}

void Interpreter::runStruct(Stmt* stmt)
{
    // Currently does nothing
}

void Interpreter::runEnum(Stmt* stmt)
{
    // Currently does nothing
}

void Interpreter::runFunc(Stmt* stmt)
{
    // Currently does nothing
}

void Interpreter::runReturn(Stmt* stmt)
{
    Return* r = asReturn(stmt);
    returnValue = evaluateExpr(r->expr);
    shouldReturn = true;
}

void Interpreter::runIf(Stmt* stmt)
{
    If* i = asIf(stmt);

    if (isTruthy(evaluateExpr(i->condition))) {
        runStmt(i->ifBody);
    } else {
        if (i->elseBody) runStmt(i->elseBody);
    }
}

void Interpreter::runExprStmt(Stmt* stmt)
{
    ExprStmt* exprStmt = asExprStmt(stmt);
    evaluateExpr(exprStmt->expr);
}

void Interpreter::runDefer(Stmt* stmt)
{
    Defer* defer = asDefer(stmt);

    auto vec = deferStatements.top();
    vec.push_back(defer->block);
    deferStatements.pop();
    deferStatements.push(vec);
}

void Interpreter::runFor(Stmt* stmt)
{
    pushDefers();

    For* forLoop = asFor(stmt);
    
    Any one;
    one.type.base = Type::INT;
    one.value.Int = 1;

    Any index = evaluateExpr(forLoop->start);
    Any end = evaluateExpr(forLoop->end);

    variables[forLoop->it] = index;

    while( isTruthy( index.less(end) ) ) {
        runStmt(forLoop->body);
        
        if (shouldReturn || shouldBreak) break;

        index = index.add(one);
        variables[forLoop->it] = index;
    }
    
    variables.erase(forLoop->it);
    shouldBreak = false;
    executeDefers();
}

void Interpreter::runWhile(Stmt* stmt)
{
    pushDefers();

    While* whileLoop = asWhile(stmt);

    while( isTruthy( evaluateExpr(whileLoop->condition) ) ) {
        runStmt(whileLoop->body);
        
        if (shouldReturn || shouldBreak) break;
    }

    shouldBreak = false;
    executeDefers();
}

void Interpreter::runAssign(Stmt* stmt)
{
    Assign* assign = asAssign(stmt);

    Ident* left = asIdent(assign->left);

    if (!variables.contains(left->name)) error("Undefined Variable");

    variables[left->name] = evaluateExpr(assign->right);
}

void Interpreter::runSet(Stmt* stmt)
{
    Set* set = asSet(stmt);

    Any any     = evaluateExpr(set->expr);
    Any access  = evaluateExpr(set->access);
    Any value   = evaluateExpr(set->value);

    if (any.type.flags & Flags::ARRAY) {

        any.setArrayMember(access.value.Int, value);

    } else if (any.type.base == Type::STRUCT) {

        any.setStructMember(*access.value.String, value);

    } else if (any.type.base == Type::ENUM) {

        //TODO: See above

    } else {
        error("Set should only be called from arrays or structs or enums");
    }
}

void Interpreter::runContinue(Stmt* stmt)
{
    shouldContinue = true;
}

void Interpreter::runBreak(Stmt* stmt)
{
    shouldBreak = true;
}

bool Interpreter::isTruthy(Any any)
{
    switch(any.type.base) {
        case(Type::FLOAT):
        case(Type::DOUBLE):
        case(Type::INT):
        case(Type::CHAR):
        case(Type::NUMBER):
        case(Type::S64):
        case(Type::S32):
        case(Type::S16):
        case(Type::S8):
        case(Type::U64):
        case(Type::U32):
        case(Type::U16):
        case(Type::U8): return any.value.Int != 0;
        case(Type::BOOL): return any.value.Bool;

        default: return false;
    }
}

Any Interpreter::evaluateExpr(Expr* expr)
{
    switch(expr->kind) {
        case(ET::EXPR):     INTERNAL_ERROR("We shouldn't have wild normal Exprs running around but here we are.");
        case(ET::CONST):    return evaluateConst(expr);
        case(ET::BINARY):   return evaluateBinary(expr);   
        case(ET::UNARY):    return evaluateUnary(expr);
        case(ET::IDENT):    return evaluateIdent(expr);
        case(ET::CALL):     return evaluateCall(expr);
        case(ET::GET):      return evaluateGet(expr);
    }

    INTERNAL_ERROR("NO EXPR MATCHED THIS SWITCH");
    Any any;
    return any;
}

Any Interpreter::evaluateConst(Expr* expr)
{
    Const* c = asConst(expr);

    return c->any;
}

Any Interpreter::evaluateBinary(Expr* expr)
{
    Binary* binary = asBinary(expr);

    Any left = evaluateExpr(binary->left);
    Any right = evaluateExpr(binary->right);

    Any result;
    switch(binary->op) {
        case(OP::EQUAL): return left.equal(right); 
        case(OP::NOT_EQUAL):        return left.notEqual(right);
    
        case(OP::AND):              return left.And(right);
        case(OP::OR):               return left.Or(right);

        case(OP::GREATER):          return left.greater(right);
        case(OP::GREATER_EQUAL):    return left.greaterEqual(right);
        case(OP::LESS):             return left.less(right);
        case(OP::LESS_EQUAL):       return left.lessEqual(right);
    
        case(OP::MINUS):            return left.sub(right);
        case(OP::PLUS):             return left.add(right);
        case(OP::MULTIPLY):         return left.mul(right);
        case(OP::DIVIDE):           return left.div(right);
        default: {
            error("Binary Expression has an unknown Operator");
        } 
    }
    return result;
}

Any Interpreter::evaluateUnary(Expr* expr)
{
    Unary* unary = asUnary(expr);

    Any result;
    switch(unary->op) {
        case(OP::NOT):      result = evaluateExpr(unary->expr).Not(); break;
        case(OP::NEGATE):   result = evaluateExpr(unary->expr).neg(); break;

        default: INTERNAL_ERROR("Wrong Unary Operators shouldn't get parsed")
    }
    return result;
}

Any Interpreter::evaluateIdent(Expr* expr)
{
    Ident* ident = asIdent(expr);

    if (variables.contains(ident->name)) return variables[ident->name];

    if (constants.contains(ident->name)) return constants[ident->name];

    error("Undefined Variable");
    
    Any any;
    return any;
}

Any Interpreter::evaluateCall(Expr* expr)
{
    Call* call = asCall(expr);

    //TODO:    For now we only support direct Function calls and not Functions in Structs
    //CLEANUP: ALLOWED: func(a, b)    NOT_ALLOWED: vec.add(3) 
    Ident* ident = asIdent(call->name);

    if (!functions.contains(ident->name)) error("Fucntion not defined");
    Func* defn = functions[ident->name];

    if (defn->params.size() != call->args.size()) error("Wrong number of arguments");

    // Putting the arguments into the variables for the function
    for (int i = 0; i < call->args.size(); i++) {
        auto arg = call->args[i];
        Any value = evaluateExpr(arg);
        variables[defn->params[i]->name] = value;
    }

    Any result = callFunction(defn);

    // Removing the arguments after the function is done;
    for (auto param : defn->params) {
        variables.erase(param->name);
    }

    return result;
}

Any Interpreter::evaluateArray(Expr* expr)
{
    Any any;
    return any;
}

Any Interpreter::evaluateGet(Expr* expr)
{
    Get* get = asGet(expr);

    Any any = evaluateExpr(get->expr);
    Any access = evaluateExpr(get->access);

    if (any.type.flags & Flags::ARRAY) {

        return any.getArrayMember(access.value.Int);

    } else if (any.type.base == Type::STRUCT) {

        // INFO: We mark the get->expr as STRUCT 
        // even though we don't know if it's a struct or an enum
        // so we have to check first what it is bevore we access it.

        return any.getStructMember(*access.value.String);

    } else if (any.type.base == Type::ENUM) {

        //TODO: See above

    } else {
        error("Get should only be called from arrays or structs or enums");
    }

    return any;
}