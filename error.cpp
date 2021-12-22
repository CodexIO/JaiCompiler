#include <string>
#include <iostream>
#include <vector>

#include "Token.h"

using namespace std;

void error(string errorMessage)
{
    cout << "------------------------- ERROR: -------------------------\n" << errorMessage << endl;
    exit(-1);
}

void error(string errorMessage, Token tk)
{
    cout << "\n--------------------- ERROR: ---------------------" << endl;
    cout << "On line: " << tk.lineNumber << " column: " << tk.coulmnNumber << endl;
    cout << errorMessage << endl;
    exit(-1);
}


#define ERROR(after, expected, tk) \
    stringstream message;  \
    message << "Expected: ";  \
    for (string s : expected) message << "'" << s << "',"; \
    message << " After: " << after << "\nFound: " << tk.type << " with: " << tk.source << endl; \
    message << "DEBUG: Code in File: " << __FILE__ << " at Line: " << __LINE__ << " produced error." << endl; \
    error(message.str(), tk); \
    exit(-1); 
