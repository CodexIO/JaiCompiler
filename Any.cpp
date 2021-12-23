#include <string>
#include <iostream>
#include <sstream>

#include "Any.h"
#include "error.h"
#include "Stmt.h"

using std::cout, std::endl, std::string, std::stringstream;

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
        PROCESS_VAL(STRUCT)
        PROCESS_VAL(FUNCTION)

        PROCESS_VAL(UNKNOWN)
    }
#undef PROCESS_VAL
    return std::string(s);
}


ImprovedType::ImprovedType(Type t, TypeFlags f) : base(t), flags(f)
{

}

MyStruct::MyStruct(Struct* d) : defn(d)
{
    int count = d->memberPositions.size();
    members.reserve(count);

    for (int i = 0; i < count; i++) {
        Any any;
        any.type = d->memberTypes[i];
        any.value.Int = 0; //CLEANUP Strings are currently not initialised;
        members.push_back(any);
    }
}

Any MyStruct::get(string name)
{
    if (!defn->memberPositions.contains(name)) error("Struct member not defined.(reading)");

    int index = defn->memberPositions[name];

    return members[index];
}

void MyStruct::set(string name, Any& any)
{
    if (!defn->memberPositions.contains(name)) error("Struct member not defined.(writing)");

    int index = defn->memberPositions[name];

    members[index] = any;
}


MyArray::MyArray(ImprovedType t, int size) : type(t)
{
    values.reserve(size);
    for (int i = 0; i < size; i++) {
        Any any;
        any.type = t;
        any.value.Int = 0;
        values.push_back(any);
        // CLEANUP: Right now Strings wouldn't have a default value in the Array
    }
}

Any MyArray::get(int index)
{
    if (index >= values.size()) error("ARRAY INDEX OUT OF BOUNDS EXCEPTION (reading)");

    return values[index];
}

void MyArray::set(int index, Any& any)
{
    if (index >= values.size()) error("ARRAY INDEX OUT OF BOUNDS EXCEPTION (writing)");

    values[index] = any;
}

Value::Value() : Int(0) {

}

Any::~Any()
{
    //cout << "DESTRUCTOR CALLED" << endl;
    anyCount;
    //TODO Figure out how to do this!
    if (this->type.base == Type::STRING) delete this->value.String;
}

Any::Any() : type(Type::UNKNOWN, 0), value() {
    //cout << "NORMAL CONSTRUCTOR CALLED" << endl;
    anyCount++;
}


Any::Any(Any&& other) : type(other.type) {
    //cout << "MOVE CONSTRUCTOR CALLED" << endl;
    value = other.value;

    other.value.String = nullptr;
}

Any::Any(const Any& other) : type(other.type)
{
    anyCount++;
    //cout << "COPY CONSTRUCTOR CALLED" << endl;
    if (type.base == Type::STRING) {
        value.String = new string(*other.value.String);
    }  else {
        value = other.value;
    }
}

Any& Any::operator=(const Any& other)
{
    //cout << "Assignment Called"  << endl;
    this->type = other.type;

    if (this->type.base == Type::STRING) delete this->value.String;

    
    if(other.type.base == Type::STRING) {
        string* newString = new string(*other.value.String);
        
        this->value.String = newString;
    } else {
        this->value = other.value;
    }
    return *this;
}

#define DO_BASIC_MATH(left, op, right) \
    Any any; \
    any.type = left->type; \
    switch (left->type.base) { \
        case(Type::FLOAT): \
        case(Type::DOUBLE): { \
            any.value.Float = left->value.Float op right.value.Float;  \
        } break; \
        case(Type::CHAR): \
        case(Type::INT): \
        case(Type::NUMBER): \
        case(Type::S64): \
        case(Type::S32): \
        case(Type::S16): \
        case(Type::S8): \
        case(Type::U64): \
        case(Type::U32): \
        case(Type::U16): \
        case(Type::U8): { \
            any.value.Int = left->value.Int op right.value.Int; \
        } break; \
        default: { \
            error("Wrong Arguments for binary Math Operator"); \
        } \
    } \
    return any;

#define DO_BASIC_BOOL(left, op, right) \
    Any any; \
    any.type.base = Type::BOOL; \
    switch (left->type.base) { \
        case(Type::FLOAT): \
        case(Type::DOUBLE): { \
            any.value.Bool = left->value.Float op right.value.Float;  \
        } break; \
        case(Type::CHAR): \
        case(Type::INT): \
        case(Type::NUMBER): \
        case(Type::S64): \
        case(Type::S32): \
        case(Type::S16): \
        case(Type::S8): \
        case(Type::U64): \
        case(Type::U32): \
        case(Type::U16): \
        case(Type::U8): { \
            any.value.Bool = left->value.Int op right.value.Int; \
        } break; \
        default: { \
            error("Wrong Arguments for binary Bool Operator"); \
        } \
    } \
    return any;

string Any::toString() const
{
    switch (this->type.base) {
        case(Type::STRING): return *this->value.String;
        case(Type::CHAR): return string(1, this->value.Char);
        case(Type::BOOL): return this->value.Bool ? "true" : "false";
        case(Type::FLOAT):
        case(Type::DOUBLE): {
            stringstream s;
            s << this->value.Float;
            return s.str();
        }

        case(Type::INT):
        case(Type::NUMBER):
        case(Type::S64):
        case(Type::S32):
        case(Type::S16):
        case(Type::S8):
        case(Type::U64):
        case(Type::U32):
        case(Type::U16):
        case(Type::U8): {
            stringstream s;
            s << this->value.Int;
            return s.str();
        }
    }
    return "";
}

Any Any::add(const Any& other) 
{
    
    if (this->type.base == Type::STRING) {
        Any any;
        any.type = this->type;
        any.value.String = new string(*this->value.String + other.toString());
        return any;
    }
    DO_BASIC_MATH(this, +, other);
}

Any Any::sub(const Any& other) 
{
    DO_BASIC_MATH(this, -, other)
}

Any Any::mul(const Any& other) 
{
    DO_BASIC_MATH(this, *, other)
}

Any Any::div(const Any& other) 
{
    DO_BASIC_MATH(this, /, other)
}

Any Any::equal(const Any& other) 
{
    if (this->type.base == Type::STRING) {
        Any any;
        any.type.base = Type::BOOL;
        any.value.Bool = *this->value.String == *other.value.String;
        return any;
    }
    DO_BASIC_BOOL(this, ==, other)    
}

Any Any::notEqual(const Any& other) 
{
    if (this->type.base != Type::STRING) {
        Any any;
        any.type.base = Type::BOOL;
        any.value.Bool = *this->value.String == *other.value.String;
        return any;
    }
    DO_BASIC_BOOL(this, !=, other)
}

Any Any::greater(const Any& other) 
{
    DO_BASIC_BOOL(this, >, other)
}

Any Any::greaterEqual(const Any& other) 
{
    DO_BASIC_BOOL(this, >=, other)
}

Any Any::less(const Any& other) 
{
    DO_BASIC_BOOL(this, <, other)
}

Any Any::lessEqual(const Any& other) 
{
    DO_BASIC_BOOL(this, <=, other)
}

Any Any::And(const Any& other)
{
    DO_BASIC_BOOL(this, &&, other)
}

Any Any::Or(const Any& other)
{
    DO_BASIC_BOOL(this, ||, other)
}

Any Any::neg() 
{
    Any any;
    any.type = this->type;
    switch (this->type.base) { 
        case(Type::FLOAT): 
        case(Type::DOUBLE): { 
            any.value.Float = - this->value.Float;
        } break; 
        case(Type::CHAR): 
        case(Type::INT): 
        case(Type::NUMBER): 
        case(Type::S64): 
        case(Type::S32): 
        case(Type::S16): 
        case(Type::S8): 
        case(Type::U64): 
        case(Type::U32): 
        case(Type::U16): 
        case(Type::U8): { 
            any.value.Int = - this->value.Int;
        } break; 
        default: { 
            error("Wrong Arguments for Unary Neg Operator"); 
        } 
    }
    return any;
}

Any Any::Not() 
{
    ASSERT(this->type.base == Type::BOOL)
    Any any;
    any.type = this->type;
    switch (this->type.base) { 
        case(Type::BOOL): {
            any.value.Bool = !this->value.Bool;
        } break;
        default: { 
            error("Wrong Arguments for Unary Not Operator"); 
        } 
    }
    return any;
}

Any Any::getArrayMember(int index)
{
    if (!(type.flags & Flags::ARRAY)) error("tryed to index something that isn't an Array (reading)");

    MyArray* array = (MyArray*) value.Ptr;

    return array->get(index);
}

void Any::setArrayMember(int index, Any& any)
{
    if (!(type.flags & Flags::ARRAY)) error("tryed to index something that isn't an Array (writing)");

    MyArray* array = (MyArray*) value.Ptr;

    array->set(index, any);
}

Any Any::getStructMember(string name)
{
    if (type.base != Type::STRUCT) error("tried to index something that isn't a Struct (reading)");

    MyStruct* st = (MyStruct*) value.Ptr;

    return st->get(name);
}

void Any::setStructMember(string name, Any& any)
{
    if (type.base != Type::STRUCT) error("tried to index something that isn't a Struct (writing)");

    MyStruct* st = (MyStruct*) value.Ptr;

    return st->set(name, any);
}

Any Any::getEnumValue(string name)
{
    if (type.base != Type::ENUM) error("tried to index something that isn't an Enum");

    Enum* en = (Enum*) value.Ptr;

    Any any;
    any.type = ImprovedType(Type::INT);
    any.value.Int = en->values[name];

    return any;
}