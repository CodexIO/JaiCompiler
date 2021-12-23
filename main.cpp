#include <iostream>
#include <string>

#include "Parser.h"
#include "Expr.h"
#include "Interpreter.h"

using namespace std;

int Any::anyCount = 0;

void anyTest()
{
    cout << "Testing Any" << endl;

    unordered_map<string, Any> variables{0};

    Any any;
    any.type = Type::STRING;
    any.value.String = new string("Hello World");

    Any other = any;

    Any foo = any.add(other);

    cout << "AnyCount = " << Any::anyCount << endl;
}

int main()
{
    const char* file = "jai_syntax.jai";

    cout << "Compiling: " << file << endl;

    Parser parser(file);

    parser.parse();

    Interpreter interp(parser);

    interp.run();

    cout << "AnyCount = " << Any::anyCount << endl;
}