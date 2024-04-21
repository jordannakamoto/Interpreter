#ifndef RECURSIVE_DESCENT_PARSER_H
#define RECURSIVE_DESCENT_PARSER_H

#include "ConcreteSyntaxTree.h"
#include "TokenList.h"
#include "utilities/IdentifierClassifier.h"
#include "ErrorHandler.h"

class RecursiveDescentParser {

using IDis = IdentifierClassifier;

public:
    ErrorHandler& eh = ErrorHandler::get(); // error handler reference
    int lineNumber;
    TokenList* token_list;
    TokenList::Node* curr;
    ConcreteSyntaxTree* cst;
    Token_Type tt;    // tt
    std::string tv;   // tokenValue
    bool nextIsChild = false; // Set next sib call to actually create a child like when we hit a semicolon and need the next node to be added as a child

    // Aliases
    Token_Type id = IDENTIFIER;
    Token_Type lp = LEFT_PARENTHESIS;
    Token_Type rp = RIGHT_PARENTHESIS;

    void printCurrType();
    void printCurrValue();

    void sib();
    void child();
    void setNextSibAsChild();

    // Constructor Initial Setup
    RecursiveDescentParser(TokenList* tl);

    ConcreteSyntaxTree* LinkedListToCST();

    // *** r *** //
    // Each BNF form has it's own function prepended by 'r' for recursive
    void rProcedure();
    void rFunction();
    void rParameterList();

    // STATEMENTS

    // rBlockStatement, block of statements {}
    void rBlockStatement();

    // rCompoundStatement, Set of statement lines inside a function or procedure block{...}
    void rCompoundStatement();

    // rStatement, Branches into all types of statements
    void rStatement();
    void rDeclarationStatement();

    void rIdentifierAndArrayList();
    void rIterationStatement();
    void rSelectionStatement();
    void rPrintfStatement();
    void rReturnStatement();
    void rAssignmentStatement();
    void rUserDefinedFunction();

    void rExpression();
    void rBooleanExpression();
    void rInitializationExpression();

    // HELPERS
    bool isBooleanOperator(Token_Type tokenType);

    bool isNumericalOperator(Token_Type tokenType);
    
    void throwDebug(std::string msg);
    void throwDebug();

    void rSingleDoubleString();

// private:
};

#endif // RECURSIVE_DESCENT_PARSER_H

/* ERRORS TO CATCH ASSIGNMENT 3
Syntax error on line 8: unterminated string quote.
//eh.exitUnterminatedStr(lineNumber)
Syntax error on line 6: array declaration size must be a positive integer.
// eh.exitNegativeArray(lineNumber)
Syntax error on line 6: reserved word "char" cannot be used for the name of a variable.
Syntax error on line 6: reserved word "void" cannot be used for the name of a variable.
Syntax error on line 12: reserved word "int" cannot be used for the name of a variable.
// eh.exitVarName(ln,tokenValue)
Syntax error on line 12: reserved word "printf" cannot be used for the name of a function.
// eh.exitFuncName(ln,tokenValue)
*/