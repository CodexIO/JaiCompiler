#pragma once

#include <string>
#include <vector>
#include <tuple>

struct Struct;

namespace Flags {
    const int POINTER = 1;
    const int ARRAY = 2;
    const int CONSTANT = 4;
}

union Value {
        long long int Int;
        double Float;
        bool Bool;
        char Char;
        std::string* String;
        void* Ptr;
        Value();
};

typedef int TypeFlags;

enum class Type {
    NUMBER,
    STRING,
    FLOAT,
    DOUBLE,
    INT,
    CHAR,
    BOOL,

    S64,
    S32,
    S16,
    S8,
    U64,
    U32,
    U16,
    U8,

    VOID,
    ENUM,
    STRUCT,
    FUNCTION,

    TO_INFER,
    UNKNOWN
};

std::string TypeToString(Type type);

// CLEANUP: This system will have problems with arrays of pointers or pointers to arrays
struct ImprovedType {
    Type base = Type::UNKNOWN;
    TypeFlags flags = 0;
    void* info;

    ImprovedType(Type t, TypeFlags f = 0);
};

struct Any {
    ImprovedType type;
    Value value;

    //CLEANUP remove this
    static int anyCount; 

    Any();
    Any(Any&& other);
    Any(const Any& other);
    ~Any();

    Any& operator=(const Any& other);

    std::string toString() const;

    Any add(const Any& other);
    Any sub(const Any& other);
    Any mul(const Any& other);
    Any div(const Any& other);
    
    Any equal(const Any& other);
    Any notEqual(const Any& other);
    Any greater(const Any& other);
    Any greaterEqual(const Any& other);
    Any less(const Any& other);
    Any lessEqual(const Any& other);
    
    Any And(const Any& other);
    Any Or(const Any& other);
    
    Any neg();
    Any Not();

    // Functions for when its an Array
    Any  getArrayMember(int index);
    void setArrayMember(int index, Any& any);
    // Functions for when its a Struct

    Any getStructMember(std::string name);
    void setStructMember(std::string name, Any& any);
};

struct MyStruct {
    Struct* defn;
    std::vector<Any> members;

    MyStruct(Struct* d);

    Any  get(std::string name);
    void set(std::string name, Any& any);
};

struct MyArray {
    ImprovedType type;
    std::vector<Any> values;

    MyArray(ImprovedType type, int size);
    
    Any  get(int index);
    void set(int index, Any& any);
};