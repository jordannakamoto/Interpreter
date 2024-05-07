#include "ErrorHandler.h"

// Call this method to generate an output file for a caught error...
void ErrorHandler::outputErrorToFile(std::string errorMessage) {
    std::string outputName = generateOutputFileName();
    std::ofstream outputFile(outputName); // Open File

    if(outputFile.is_open()) {
        outputFile << errorMessage << "\n";
        outputFile.close();
    }
}

/* --------------------------------------------------------------------------- *
 *  Assignment 3 Errors:                                                       *
/ ---------------------------------------------------------------------------  */

void ErrorHandler::exitNegativeArray(int lineNumber)
{
    std::string message = "Syntax error on line " + std::to_string(lineNumber) + ": array declaration size must be a positive integer.";
    std::cerr << message << std::endl;
    outputErrorToFile(message);
    exit(12);
}

void ErrorHandler::exitUnterminatedStr(int lineNumber)
{
    std::string message = "Syntax error on line " + std::to_string(lineNumber) + ": unterminated string quote.";
    std::cerr << message << std::endl;
    outputErrorToFile(message);
    exit(12);
    
}

// CST, exit on bad variable naming
// - If a reserved word attempted to be used as a name 
//   for a variable, procedure, function
void ErrorHandler::exitVarName(int lineNumber, const std::string &tokenValue)
{
    std::string message = "Syntax error in line " + std::to_string(lineNumber) + ": reserved word \"" + tokenValue + "\" cannot be used for the name of a variable.";
    std::cerr << message << std::endl;
    outputErrorToFile(message);
    exit(12);
}
// ...
void ErrorHandler::exitProcName(int lineNumber, const std::string &tokenValue)
{
    std::string message = "Syntax error in line " + std::to_string(lineNumber) + ": reserved word \"" + tokenValue + "\" cannot be used for the name of a procedure.";
    std::cerr << message << std::endl;
    outputErrorToFile(message);
    exit(12);
}
// ...
void ErrorHandler::exitFuncName(int lineNumber, const std::string &tokenValue)
{
    std::string message = "Syntax error in line " + std::to_string(lineNumber) + ": reserved word \"" + tokenValue + "\" cannot be used for the name of a function.";
    std::cerr << message << std::endl;
    outputErrorToFile(message);
    exit(12);
}

// TODO: Should probably combine the following into a single exit function with a message string...
// CST: on bad Token Type for
// procedure, function naming
// ...
void ErrorHandler::exitProcType(int lineNumber, tdfa::Token_Type tokenType)
{
    std::string message = "Syntax error in line " + std::to_string(lineNumber) + ": token of type " + Token::tokenTypeToString(tokenType) + " cannot be used for the name of a procedure.";
    outputErrorToFile(message);
}

// ...
void ErrorHandler::exitFuncType(int lineNumber, tdfa::Token_Type tokenType)
{
    std::string message = "Syntax error in line " + std::to_string(lineNumber) + ": token of type " + Token::tokenTypeToString(tokenType) + " cannot be used for the name of a function.";
    outputErrorToFile(message);
}

// CST: exit on bad statement evaluation
void ErrorHandler::exitStatementSyntaxType(int lineNumber, std::string statementType, Token_Type expectedType, Token_Type actualType)
{
    std::string message = "Syntax Error in statement in line " + std::to_string(lineNumber) + " of type: " + statementType + ". Expected token_type: " + Token::tokenTypeToString(expectedType) + " , got: " + Token::tokenTypeToString(actualType);
    outputErrorToFile(message);
}

void ErrorHandler::exitStatementSyntaxValue(int lineNumber, std::string statementType, std::string expectedValue, std::string actualValue)
{
    std::string message = "Syntax Error in statement in line " + std::to_string(lineNumber) + " of type: " + statementType + ". Expected token_value: " + expectedValue + " , got: " + actualValue;
    outputErrorToFile(message);
}

void ErrorHandler::exitStatementIncomplete(int lineNumber, std::string message)
{
    std::string outputstring = "Incomplete statement in line " + std::to_string(lineNumber) + " : " + message;
    std::cerr << outputstring << std::endl;
    outputErrorToFile(message);
    exit(12);
}
// TODOEND

// End Assignment 3 ------------------------------------------------------------ //


// Function for adding an error to a cohesive errors list, e.g. not exiting immediately
void ErrorHandler::addError(int lineNumber, const std::string &message, int severity)
{
    errors.emplace_back(lineNumber, message, severity);
}
bool ErrorHandler::hasErrors()
{
    return !errors.empty();
}
void ErrorHandler::printErrors()
{
    for (const auto &error : errors){
        if (error.severity > 0) std::cerr << error.message << std::endl;
    }
}

// Return a reference to the last error
const ErrorHandler::Error& ErrorHandler::getLastError()
{
    return errors.back();
}

// Generate a name for the error output file...
std::string ErrorHandler::generateOutputFileName(){
    size_t lastIndex = fileName.find_last_of(".");
    std::string outputName = fileName.substr(0, lastIndex);

    // Change path to new output directory
    std::string toSearch = "inputs";
    std::string replaceWith = "program-outputs";
    size_t pos = outputName.find(toSearch);
    if (pos != std::string::npos) { // if "inputs" is found
        outputName.replace(pos, toSearch.length(), replaceWith);
    }
    std::cout << outputName << std::endl;

    outputName += "-error_output_file.txt";
    return outputName;
}