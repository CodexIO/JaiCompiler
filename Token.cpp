#include <string>
#include <iostream>

#include "Token.h"

using namespace std;

string TkTypetoString(TkType type)
{
    const char* s = "ERROR! TkType type is missing in the toStringMethod";
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(type){
        PROCESS_VAL(LOAD)
        PROCESS_VAL(STRUCT)
        PROCESS_VAL(ENUM)
        PROCESS_VAL(DEFER)
        PROCESS_VAL(FOR)
        PROCESS_VAL(WHILE)
        PROCESS_VAL(BREAK)
        PROCESS_VAL(CONTINUE)

        PROCESS_VAL(IF)
        PROCESS_VAL(ELSE)
        PROCESS_VAL(THEN)
        PROCESS_VAL(RETURN)
        PROCESS_VAL(TYPE)

        PROCESS_VAL(NULL_CONSTANT)
        PROCESS_VAL(CHAR_CONSTANT)
        PROCESS_VAL(NUMBER_CONSTANT)
        PROCESS_VAL(DOUBLE_CONSTANT)
        PROCESS_VAL(STRING_CONSTANT)
        PROCESS_VAL(BOOL_CONSTANT)
        PROCESS_VAL(IDENTIFIER)

        PROCESS_VAL(EQUAL)
        PROCESS_VAL(EQUAL_EQUAL)
        PROCESS_VAL(BANG)
        PROCESS_VAL(BANG_EQUAL)
        PROCESS_VAL(LESS)
        PROCESS_VAL(LESS_EQUAL)
        PROCESS_VAL(GREATER)
        PROCESS_VAL(GREATER_EQUAL)
        PROCESS_VAL(OR)
        PROCESS_VAL(AND)
        PROCESS_VAL(BIT_OR)
        PROCESS_VAL(BIT_AND)

        PROCESS_VAL(COMMA)
        PROCESS_VAL(POINT)
        PROCESS_VAL(POINT_POINT)
        PROCESS_VAL(COLON)
        PROCESS_VAL(COLON_COLON)
        PROCESS_VAL(SEMICOLON)
        PROCESS_VAL(ARROW)

        PROCESS_VAL(PLUS)
        PROCESS_VAL(PLUS_EQUAL)
        PROCESS_VAL(MINUS)
        PROCESS_VAL(MINUS_EQUAL)
        PROCESS_VAL(SLASH)
        PROCESS_VAL(SLASH_EQUAL)
        PROCESS_VAL(STAR)
        PROCESS_VAL(STAR_EQUAL)

        PROCESS_VAL(OPEN_PAREN)
        PROCESS_VAL(CLOSE_PAREN)
        PROCESS_VAL(OPEN_CURLY)
        PROCESS_VAL(CLOSE_CURLY)
        PROCESS_VAL(OPEN_BRACKET)
        PROCESS_VAL(CLOSE_BRACKET)

        PROCESS_VAL(UNKNOWN_TOKEN)
        PROCESS_VAL(END)
    }
#undef PROCESS_VAL

    return string(s);
}

ostream& operator<<(ostream& out, const TkType type) {
    return out << TkTypetoString(type);
}


Token::Token(TkType type, std::string source, int lN, int cN) : lineNumber(lN), coulmnNumber(cN) {
    this->type = type;
    this->source = source;
}

Token::Token() {
    this->type = TkType::UNKNOWN_TOKEN;
}

void Token::print()
{
    Token tk = *this;
    cout << tk.type << " = " << tk.source << " line: " << tk.lineNumber << " col: " << tk.coulmnNumber << endl;
}