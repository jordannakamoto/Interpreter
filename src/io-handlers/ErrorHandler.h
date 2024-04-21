// Temporary Solution for error handling
// Used to bypass error exiting from previous assignments but still record them
// Singleton Instance accessed through ErrorHandler::get()
// Example Usage: ErrorHandler::get().addError(linenumber, "This is an error message", 0) where 0 is severity level

#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <string>
#include <vector>
#include <iostream>
#include "Token.h"
#include <fstream>

class ErrorHandler {
public:
    // the name of the current open file for printing error output files
    std::string fileName; // must be assigned in main P1 after file is opened by fileHandler

    // Public method to access the singleton instance
    static ErrorHandler& get() {
        static ErrorHandler instance; // Guaranteed to be destroyed and instantiated on first use.
        return instance;
    }

    // Singleton Pattern:
    // Prevent Copy Constructor and Assignment
    // So there is only 1 global Instance of ErrorHandler
    ErrorHandler(const ErrorHandler&) = delete; 
    ErrorHandler& operator=(const ErrorHandler&) = delete;

    // DATA STRUCTURE of Single Error
    struct Error
    {
        int lineNumber;
        std::string message;
        int severity;

        Error(int line, const std::string &msg, int svr) : lineNumber(line), message(msg), severity(svr) {}
    };

    // Output Error to a File
    void outputErrorToFile(std::string errorMessage);
    std::string generateOutputFileName();

    // Assignment 3 ---------------------------------------------------------------- //
    // Assignment 3, exit on bad variable naming

    void exitNegativeArray(int lineNumber);
    void exitUnterminatedStr(int lineNumber);

    // Assignment 3 If a reserved word attempted to be used as a name
    // for a variable, procedure, function
    void exitVarName(int lineNumber, const std::string& tokenValue);
    void exitProcName(int lineNumber, const std::string& tokenValue);
    void exitFuncName(int lineNumber, const std::string& tokenValue);

    // Assignment 3 Parsing BNF: Token Type validation
    void exitProcType(int lineNumber, tdfa::Token_Type tokenType);
    void exitFuncType(int lineNumber, tdfa::Token_Type tokenType);
    void exitStatementSyntaxType(int lineNumber, std::string statementType, Token_Type expectedType, Token_Type actualType);
    void exitStatementSyntaxValue(int lineNumber, std::string statementType, std::string expectedValue, std::string actualValue);
    void exitStatementIncomplete(int lineNumber, std::string statementType);
    // End Assignment 3 ------------------------------------------------------------ //

    void addError(int lineNumber, const std::string& message, int severity);
    bool hasErrors();
    void printErrors();
    const Error& getLastError();

private:
    // Private constructor
    ErrorHandler() {}

    // Vector to store errors
    std::vector<Error> errors;
};

#endif //ERRORHANDLER_H

/* EXTENSION IDEAS*/
/*
    - Error Types: (e.g., INFO, WARNING, ERROR, FATAL)
    - Filtering during printing or processing
    - Error IDs/Categories unique IDs or categories for errors to facilitate documentation, localization... etc.
    - Custom Error Handlers interface for custom error handling strategies and allow the ErrorHandler to delegate error processing to these strategies.
    - Deferment mechanism to defer handling of errors until a specific program point, for batch processing or cumulative reporting.
    - Include additional context, such as file names or function names, to aid in debugging.
    - Thread Safety.
    - Log Files?
*/

