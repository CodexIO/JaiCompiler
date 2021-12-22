#pragma once

#include "Token.h"

#include <string>
#include <iostream>
#include <vector>

void error(string errorMessage);

void error(string errorMessage, Token tk);

#define ERROR(after, expected, tk) \
    std::stringstream message;  \
    message << "Expected: ";  \
    for (string s : expected) message << "'" << s << "',"; \
    message << " After: " << after << "\nFound: " << tk.type << " with: " << tk.source << endl; \
    message << "DEBUG: Code in File: " << __FILE__ << " at Line: " << __LINE__ << " produced error." << endl; \
    error(message.str(), tk); \
    exit(-1); 

#define ASSERT(x) \
if (!(x)) { \
    std::cout << "File: " << __FILE__ << " Line: " << __LINE__ <<  " Assertion failed " << endl; \
    exit(-2); \
}

#define INTERNAL_ERROR(message) \
    INTERNAL_ERROR_3(message, __FILE__, __LINE__)

#define INTERNAL_ERROR_3(message, file, line) \
    cout << "-----------------Internal Compiler Error:-----------------\n"; \
    cout << "At File: " <<  file << " Line: " << line << "\n" << message << endl; \
    exit(-3);

