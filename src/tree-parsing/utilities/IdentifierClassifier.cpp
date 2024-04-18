#include "IdentifierClassifier.h"

// isValidName()
// an IDENTIFIER token is safe to use as a variable name
// if it is not a part of any of the reserved keyword groups defined below

bool IdentifierClassifier::name(const std::string& identifier) {
    // Variable or function/procedure name is considered valid if it does not match any of the specific reserved keyword groups
    return !(function(identifier) || procedure(identifier) ||
             data(identifier) || control(identifier) || subroutine(identifier));
}

// --- Reserved Keyword Groups ---

// function and procedure Identifiers should exist in global space
// as the first token in a declaration or definition statement

// Data Identifiers:
// int, char, bool, void // note, void may not be needed as I think it may be handled explicitly

// Control Flow Identifiers:
// if, else, while, for, return

// SubRoutine Identifiers:
// printf, getchar

// --- Member Function Definitions ---

// function
bool IdentifierClassifier::function(const std::string& identifier) {
    return identifier == "function";
}

// procedure
bool IdentifierClassifier::procedure(const std::string& identifier) {
    return identifier == "procedure";
}

// Data Identifiers are base data types, modifier keywords related to data types
bool IdentifierClassifier::data(const std::string& identifier) {
    static const std::unordered_set<std::string> dataTypes = {
        // limited typeset defined in BNF:
        "int", "char", "bool", "void"
    };
    return dataTypes.find(identifier) != dataTypes.end();
}

// Control Flow Identifiers are control flow reserved words + other keywords related to control flow
bool IdentifierClassifier::control(const std::string& identifier) {
    static const std::unordered_set<std::string> controlFlows = {
        // limited controlflow set defined in BNF:
        "if", "else", "while", "for", "return"
    };
    return controlFlows.find(identifier) != controlFlows.end();
}

// SubRoutine Identifiers are identifiers of subroutines
// like printf, malloc, system
bool IdentifierClassifier::subroutine(const std::string& identifier) {
    static const std::unordered_set<std::string> std = {
        // limited subroutine set defined in BNF
        "printf", "getchar"
    };
    return std.find(identifier) != std.end();
}

// NOTES:
// --- Examples of more complete Identifier Sets --- //

// Example of complete typeset...
        // "double", "float", "long", "short"
        // "unsigned", "signed", "const", "static", "volatile", 
        // "auto", "register", "extern"

// Example of complete controlflowset...
        // , "do",
        // "switch", "case", "default",
        // "break", "continue", "goto", "exit"

// Example of complete CStd Subroutines....
/*      "scanf", "puts", "gets", "sprintf", "fprintf", "fscanf",
        "strcpy", "strncpy", "strcat", "strncat", "strcmp", "strncmp", "strlen", 
        "strstr", "strchr", "strrchr", "strtok", "strerror", "strdup",
        "malloc", "calloc", "realloc", "free",
        "abs", "labs", "div", "ldiv", "pow", "sqrt", "log", "exp", "sin", "cos", "tan",
        "atoi", "atol", "strtol", "strtod", "rand", "srand", "qsort", "bsearch",
        "time", "asctime", "strftime", "gmtime", "localtime", "mktime",
        "atexit", "system", "getenv"*/

/* Language doesn't support data structures...
// Struct Identifiers are keywords related to C data structures
// like struct, new, namespace
bool IdentifierClassifier::isStructIdentifier(const std::string& identifier) {
    static const std::unordered_set<std::string> structs = {
        "struct", "enum", "union", "class", "interface", "namespace", "template",
        "virtual", "override", "final", "static", "const", "volatile", "mutable", "new", "delete"
    };
    return structs.find(identifier) != structs.end();
}
*/