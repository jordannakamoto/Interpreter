#include "RecursiveDescentParser.h"
// Toggle Debug Message Tags

void RecursiveDescentParser::printCurrType(){
    //std::cout << curr->getToken()->getTokenTypeString() << " ";
}

void RecursiveDescentParser::printCurrValue(){
    // std::cout << curr->getToken()->getTokenValue() << " ";
}

void RecursiveDescentParser::throwDebug(std::string msg){
    // std::cout << msg << std::endl;
}
void RecursiveDescentParser::throwDebug(){
    // std::cout << "test" << std::endl;
}

// Constructor Initial Setup
// Feed tokens from TokenList
// Initialize aliases tt and tv, lineNumber
RecursiveDescentParser::RecursiveDescentParser(TokenList* tl){
    lineNumber = 1;
    curr = tl->getHead();
    tt = curr->getToken()->getTokenType();
    tv = curr->getToken()->getTokenValue();
    lineNumber = curr->getToken()->getTokenLineNumber();
    cst = new ConcreteSyntaxTree();
};

void RecursiveDescentParser::sib(){
    if(nextIsChild){
        child();
        nextIsChild = false;
        return;
    }
    else{
        cst->addNodeSibling(curr->getToken());
    }
    // debug
    // printCurrType();
    // printCurrValue();
    curr = curr->getNext();
    if(curr == NULL) return;
    tt = curr->getToken()->getTokenType();
    tv = curr->getToken()->getTokenValue();
    lineNumber = curr->getToken()->getTokenLineNumber();
}
void RecursiveDescentParser::child(){
    cst->addNodeChild(curr->getToken());
    //debug
    // printCurrType();
    // printCurrValue();
    curr = curr->getNext();
    if(curr == NULL) return;
    tt = curr->getToken()->getTokenType();
    tv = curr->getToken()->getTokenValue();
    lineNumber = curr->getToken()->getTokenLineNumber();
}

void RecursiveDescentParser::setNextSibAsChild(){
    nextIsChild = true;
}

ConcreteSyntaxTree* RecursiveDescentParser::LinkedListToCST(){
    while(curr != NULL){
        if(tt == id){
                if(IDis::function(tv)){ // If the IDENTIFIER is classified as a procedure
                    rFunction();
                }
                else if(IDis::procedure(tv)){ // If the IDENTIFIER is classified as a procedure
                    rProcedure();
                }
                // add global variable declaration detection for test 4
                // char announcement[2048];
                else{
                    rDeclarationStatement();
                }
            }
        }
    // cst->printList();
    return cst;
};

// *** r *** //
// Each BNF form has it's own function prepended by 'r' for recursive

// procedure
// <PROCEDURE_DECLARATION> ::=
//      procedure <IDENTIFIER> <L_PAREN> <PARAMETER_LIST> <R_PAREN> < L_BRACE> <COMPOUND_STATEMENT> <R_BRACE> |
//      procedure <IDENTIFIER> <L_PAREN> void <R_PAREN> < L_BRACE> <COMPOUND_STATEMENT> <R_BRACE>
// <MAIN_PROCEDURE> ::=
//      procedure main <L_PAREN> void <R_PAREN> <BLOCK_STATEMENT>
void RecursiveDescentParser::rProcedure(){
    sib(); // procedure
    if(tt == id){
        if(tv == "main"){ // branch <MAIN_PROCEDURE>
            sib(); // main
            if(tt == lp){ // L_PAREN
                sib();
                if(tv == "void"){
                    sib(); // void 
                    if(tt == rp){ // R_PAREN
                        sib();
                        rBlockStatement();
                    }   // <MAIN_PROCEDURE> done
                }
            }
        }
        else if(IDis::name(tv)){ // if tokenValue is a valid non-reserved name
            sib();               // ... consume named ID
            if(tt == lp){
                sib();
                if(IDis::data(tv)|| tt == rp){ // verify token is DATATYPE int, char, bool, void
                    if(tv != "void"){ // branch to PARAMETER_LIST
                        rParameterList();
                    }
                    else{ 
                        sib();        // if it's void, just add it
                    }
                    // <R_PAREN> < L_BRACE> <COMPOUND_STATEMENT> <R_BRACE> remain to complete both branches
                    if(tt == rp) sib();
                    if(tt == LEFT_BRACE) child();
                    setNextSibAsChild();
                    rCompoundStatement();
                    if(tt == RIGHT_BRACE) child();
                    return; // <PROCEDURE_DECLARATION> done
                }
                else{
                    // error
                }
            }
        }
    }
    else{
        // error, procedure not followed by an ID
    }
};


/*
rFunction
function <DATATYPE_SPECIFIER> <IDENTIFIER> <L_PAREN> <PARAMETER_LIST> <R_PAREN> < L_BRACE> <COMPOUND_STATEMENT> <R_BRACE> |
function <DATATYPE_SPECIFIER> <IDENTIFIER> <L_PAREN> void <R_PAREN> < L_BRACE> <COMPOUND_STATEMENT> <R_BRACE>*/
void RecursiveDescentParser::rFunction(){
    sib(); // function
    if(IDis::data(tv)){
        sib(); // DATATYPE
        if(IDis::name(tv)){
            sib(); // IDENTIFIER
            if(tt == lp){
                sib(); // L_PAREN
                // now branch into either void or parameter list
                if(IDis::data(tv) || tt == rp){ // verify token is DATATYPE int, char, bool, void
                    if(tv != "void"){ // branch to PARAMETER_LIST
                        rParameterList();
                    }
                    else{ 
                        sib();        // if it's void, just add it
                    }
                    // <R_PAREN> < L_BRACE> <COMPOUND_STATEMENT> <R_BRACE> remain to complete both branches
                    if(tt == rp) sib();
                    if(tt == LEFT_BRACE) child();
                    setNextSibAsChild();
                    rCompoundStatement();
                    if(tt == RIGHT_BRACE) child();
                    setNextSibAsChild();
                    return; // <FUNCTION_DECLARATION> done
                }
                else{
                    // error
                }
                
            }
        }
        else{ 
            // !THROW ERROR 10 error on reserved function name
            eh.exitVarName(lineNumber, tv);
        }
    }
    // handle DATATYPE error 
}

void RecursiveDescentParser::rParameterList(){
// <DATATYPE_SPECIFIER> <IDENTIFIER> |
// <DATATYPE_SPECIFIER> <IDENTIFIER> <PARAMETER_LIST>

/*
Parameter List
should probably support commas, and also the test4 requires a
function bool empty_string (char string[4096])
*/

// Added rule for array declaration in parameter list
    
    sib(); // consume DATATYPE_SPECIFIER
    if(IDis::name(tv)){
        sib(); // Consume IDENTIFIER
        if(tt == LEFT_BRACKET){ // support arrays
            rIdentifierAndArrayList();
        }
    }
    else{
        // bad param list
        // !THROW ERROR 9 throw error on reserved naming attempt
        eh.exitVarName(lineNumber, tv);
    }
    if(tt == COMMA){
        sib();
        // If we see another datatype, continue param list
        if(IDis::data(tv)){
            if(tv != "void"){
                rParameterList(); // Recurse
            }
        }
    else{
            // Syntax error bad param list
        }
    }
    
}

// STATEMENTS

// rBlockStatement, block of statements {}
void RecursiveDescentParser::rBlockStatement(){
    // <L_BRACE> <COMPOUND_STATEMENT> <R_BRACE>
    if(tt == LEFT_BRACE) child();
    setNextSibAsChild();
    rCompoundStatement();
    if(tt == RIGHT_BRACE) child();
    setNextSibAsChild();
};

// rCompoundStatement, Set of statement lines inside a function or procedure block{...}
void RecursiveDescentParser::rCompoundStatement(){
    // <STATEMENT> | <STATEMENT> <COMPOUND_STATEMENT>
    rStatement();
    if (tt == RIGHT_BRACE){ // Terminates at the end of a block }
        return;             // adding RIGHT_BRACE as a child(); handled by caller rBlockStatement
    }
    rCompoundStatement();
}

// rStatement, Branches into all types of statements
void RecursiveDescentParser::rStatement(){
    // <DECLARATION_STATEMENT> | <ITERATION_STATEMENT> | <SELECTION_STATEMENT> | <PRINTF_STATEMENT> | <RETURN_STATEMENT> |  <ASSIGNMENT_STATEMENT> |
    // Had to add <USER DEFINED FUNCTION> to BNF
    // All statement types begin with an IDENTIFIER as the first token
    if(tt == IDENTIFIER){
        if(IDis::data(tv)){ // if first token is a data type: int, bool, char, void
            rDeclarationStatement();
        }
        else if(tv=="for"){
            rIterationStatement();
        }
        else if(tv=="while"){
            rIterationStatement();
        }
        else if(tv=="if"){
            // throwDebug("entering if");
            rSelectionStatement();
        }
        else if(tv=="printf"){
            rPrintfStatement();
        }
        else if(tv=="return"){
            rReturnStatement();
        } // Final else if... if the statement starts with a named identifier
        else if(IDis::name(tv)){
            // Either an Assignment Statement
            // <IDENTIFIER> <ASSIGNMENT_OPERATOR> 
            // or User Defined Function <IDENTIFIER> <L_PAREN>

            sib(); // consume the IDENTIFIER
            if(tt == ASSIGNMENT_OPERATOR){
                rAssignmentStatement();
            }
            else if(tt == LEFT_BRACKET){ // test5 also handle array Assignments
                rAssignmentStatement();
            }
            else if(tt == lp){
                rUserDefinedFunction();
            }
            //throwDebug("done with assignment");
        }
        else{
            // type error, not an IDENTIFIER
        }
    }
}

void RecursiveDescentParser::rDeclarationStatement(){
    // <DATATYPE_SPECIFIER> <IDENTIFIER> <SEMICOLON> | <DATATYPE_SPECIFIER> <IDENTIFIER_AND_IDENTIFIER_ARRAY_LIST> <SEMICOLON>
    sib();                     // consume the <DATATYPE_SPECIFIER>
    if(!IDis::name(tv)){
            // !THROW ERROR 7, 8!
            eh.exitVarName(lineNumber, tv);
    }
    rIdentifierAndArrayList(); // expecting <IDENTIFIER> ... handles either <IDENTIFIER> + <IDENTIFIER_AND_ARRAY_LIST>
    if(tt == SEMICOLON){
        sib();
        setNextSibAsChild();
        throwDebug("semicolon dcl");
        throwDebug(tv);
    }
    else{
        // eh.exitStatementIncomplete(lineNumber, "Expected a Semicolon");
    }
};

/*
r IdentifierAndArrayList();
Example from test3.c
char hexnum [ 9 ] ;
int i , digit , number ; 
test6.c
char buffer [ -5 ] ;
*/
void RecursiveDescentParser::rIdentifierAndArrayList(){
/* <IDENTIFIER_AND_IDENTIFIER_ARRAY_LIST> ::= <IDENTIFIER_LIST> | <IDENTIFIER_ARRAY_LIST> | <IDENTIFIER_LIST> <IDENTIFIER_ARRAY_LIST> | <IDENTIFIER_ARRAY_LIST> <IDENTIFIER_LIST>
<IDENTIFIER_ARRAY_LIST> ::= <IDENTIFIER> <L_BRACKET> <WHOLE_NUMBER> <R_BRACKET> | <IDENTIFIER> <L_BRACKET> <WHOLE_NUMBER> <R_BRACKET> <COMMA> <IDENTIFIER_ARRAY_LIST> 
<IDENTIFIER_AND_IDENTIFIER_ARRAY_LIST>

Another issue with BNF:
arrays need to support identifiers that link to integers...
<L_BRACKET><IDENTIFIER>
test4: if (string[i] == '\x0')
*/
    if(tt == id){ // sometimes we start the list with a comma
        if(IDis::name(tv)) sib(); // consume the first IDENTIFIER .e.g hexnum
    }
    // Now we see if it's potentially an array or a list
    while(tt == LEFT_BRACKET || tt == COMMA) { // [ INTEGER , INTEGER]
            if(tt == LEFT_BRACKET) {
                sib(); // consume LEFT_BRACKET
                if(tt != INTEGER && tt != PLUS) {
                    // Error handling: next token should be a number for array size
                    // It could also be an Identifier that represents an integer...
                    if(tt == id){
                        if(IDis::name(tv)) sib();
                        if(tt == RIGHT_BRACKET) sib();
                    }
                }
                if(tt == PLUS){
                    sib();
                    //throwDebug("plus encountered");
                }
                if(tt == INTEGER){
                    if(std::stoi(tv) < 0){
                        // !THROW ERROR 6: Error handling for negative number array assignment
                        eh.exitNegativeArray(lineNumber);
                    }
                    else{
                        sib();
                        //throwDebug("integer encountered");
                        if(tt == RIGHT_BRACKET){
                            sib();
                            return;
                        }
                    }
                }
                //sib(); // INTEGER
                //if(tt == RIGHT_BRACKET) sib(); // RIGHT_BRACKET error unclosed array
                
            } else if(tt == COMMA) {
                sib(); // consume the COMMA to handle the next identifier
                // throwDebug("identifier list comma");
                if(IDis::name(tv)) sib(); // Valid IDENTIFIER found, potentially more arrays or identifiers to follow
            }
        }
}

void RecursiveDescentParser::rIterationStatement(){
    // for <L_PAREN> <INITIALIZATION_EXPRESSION> <SEMICOLON> <BOOLEAN_EXPRESSION> <SEMICOLON> <EXPRESSION> <R_PAREN> <STATEMENT> |
    // for <L_PAREN> <INITIALIZATION_EXPRESSION> <SEMICOLON> <BOOLEAN_EXPRESSION> <SEMICOLON> <EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> |
    // while <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> |
    // while <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT>
    if (tv == "for"){
        sib();
        if(tt == lp){
            sib();
            rInitializationExpression();
            if(tt == SEMICOLON){
                sib();
                rBooleanExpression();
                if(tt == SEMICOLON){
                    sib();
                    rExpression();
                    if(tt == rp){
                        sib();
                        if(tt == LEFT_BRACE){
                            rBlockStatement();
                        }
                        else{ // Check for ID
                            rStatement();
                        }
                    }
                }
            }
        }
    }
    else if (tv == "while"){
        sib();
        if (tt == lp){
            sib();
            // throwDebug("entering boolean expression");
            rBooleanExpression();
            if(tt == rp){
                sib();
                // throwDebug("entering right paren");
                if (tt == LEFT_BRACE){
                    // throwDebug("entering block statement");
                    rBlockStatement();
                }
                else{ // check for ID
                    rStatement();
                }
            }
        }
    }

    return;
};
    
void RecursiveDescentParser::rSelectionStatement(){
    // if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> |
    // if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> else <STATEMENT> |
    // if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> |
    // if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> else <STATEMENT> |
    // if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <BLOCK_STATEMENT> else <BLOCK_STATEMENT> |
    // if <L_PAREN> <BOOLEAN_EXPRESSION> <R_PAREN> <STATEMENT> else <BLOCK_STATEMENT>
    sib(); // consume if
    if(tt == lp){
        sib(); // <L_PAREN>
        rBooleanExpression();
        if(tt == rp){
            sib();
            if(tt == LEFT_BRACE){ // if LEFT_BRACE, start of a BLOCK_STATEMENT
                rBlockStatement();
            }
            else if(tt == id){ // assume its a STATEMENT
                rStatement();
            }
            else{
                // else type error handling
            }
            // At the end of the statement look for an else or we're done and can return
            if(tv == "else"){
                sib();
                if(tt == LEFT_BRACE){ // if LEFT_BRACE, start of a BLOCK_STATEMENT
                rBlockStatement();
            }
            else if(tt == id){ // assume its a STATEMENT
                rStatement();
            }
                // else type error handling
            }
        }
    }

    return;
};
    
void RecursiveDescentParser::rPrintfStatement(){
    // printf <L_PAREN> <DOUBLE_QUOTED_STRING> <R_PAREN> <SEMICOLON> |
    // printf <L_PAREN> <SINGLE_QUOTED_STRING> <R_PAREN> <SEMICOLON> |
    // printf <L_PAREN> <DOUBLE_QUOTED_STRING> <COMMA> <IDENTIFIER_AND_IDENTIFIER_ARRAY_LIST> <R_PAREN> <SEMICOLON> |
    // printf <L_PAREN> <SINGLE_QUOTED_STRING> <COMMA> <IDENTIFIER_AND_IDENTIFIER_ARRAY_LIST> <R_PAREN> <SEMICOLON>
    sib(); // consume printf
    if(tt == lp){     // <L_PAREN> 
        sib();
        rSingleDoubleString();
        throwDebug("printf string exited");
        if(tt == rp){ //R_PAREN
            sib();
            if(tt == SEMICOLON){
                sib();
                setNextSibAsChild();
            }
        }
        else if(tt == COMMA){
            rIdentifierAndArrayList();
            if(tt == rp){ // <R_PAREN>
                sib();
                if(tt == SEMICOLON){
                    sib();
                    setNextSibAsChild();
                }
            }
        }
        else{
            // tt error
        }
    }
    // throwDebug("printf exited");
    return;
};

void RecursiveDescentParser::rReturnStatement(){
    // return <EXPRESSION> <SEMICOLON> |
    // return <SINGLE_QUOTED_STRING> <SEMICOLON> |
    // return <DOUBLE_QUOTED_STRING> <SEMICOLON>
    sib(); // consume "return"
    if(tt == SINGLE_QUOTE){
        sib();
        if(tt == STRING){
            sib();
            while(tt == DOUBLE_QUOTE){
                sib();
            }
            if(tt == SEMICOLON){
                    sib();
            }
        }
    }
    else if(tt == DOUBLE_QUOTE){
        sib();
        if(tt == STRING){
            sib();
            while(tt == DOUBLE_QUOTE){
                sib();
            }
            if(tt == SEMICOLON){
                    sib();
            }
        }
    }
    else{
        rExpression();
    }
    if(tt == SEMICOLON){
        sib();
        //setNextSibAsChild(); rBlockStatement should close with a child } by itself
    }

    return;
};

void RecursiveDescentParser::rAssignmentStatement(){
    // <IDENTIFIER> <ASSIGNMENT_OPERATOR> <EXPRESSION> <SEMICOLON> |
    // <IDENTIFIER> <ASSIGNMENT_OPERATOR> <SINGLE_QUOTED_STRING> <SEMICOLON> |
    // <IDENTIFIER> <ASSIGNMENT_OPERATOR> <DOUBLE_QUOTED_STRING> <SEMICOLON>

    // added BNF rule for case
    // test1.c `sum = sum_of_first_n_squares ( n ) ;`
    // <IDENTIFIER> <ASSIGNMENT_OPERATOR> <EXPRESSION> <L_PAREN> <EXPRESSION> <R_PAREN> <SEMICOLON>

    // added another BNF rule for case
    // assigning to arrays test5 
    // e.g. buffer[0]

    // sib(); // previously consumed by the caller rStatement()...
    // lookahead for array
    throwDebug("assignment entered");
    if(tt == LEFT_BRACKET){
        // throwDebug("left bracket detected");
        rIdentifierAndArrayList();
    }
    if(tt == ASSIGNMENT_OPERATOR){
        sib();
        if(tt == SINGLE_QUOTE || tt == DOUBLE_QUOTE){
            rSingleDoubleString();
            setNextSibAsChild();
        }
        else{
        rExpression();
        }
    }
    if(tt == SEMICOLON){
        sib();
        setNextSibAsChild();
    }
    // Added Rule
    else if(tt == lp){
        sib();
        rExpression();
        if(tt == rp){
            sib();
            if(tt == SEMICOLON){
                sib();
                setNextSibAsChild();
            }
        }
    }
};

void RecursiveDescentParser::rUserDefinedFunction(){
    // <IDENTIFIER> <L_PAREN> <IDENTIFIER_AND_IDENTIFIER_ARRAY_LIST> <R_PAREN> |
    // <IDENTIFIER> <L_PAREN> <EXPRESSION> <R_PAREN>
    // ID has already been consumed by calling function rStatement()

    if(tt == lp) sib(); // consume L_PAREN
    // need to somehow differentiate expression or ID ARRAY LIST...
    // probably by looking ahead for an operator or a comma
    // but for now lets just add siblings until the R_PAREN
    while(tt != rp){
        sib();
    }
    sib(); // add R_PAREN
    if(tt == SEMICOLON){
        sib();
        setNextSibAsChild();
    }
    else{
        // error in UDF
    }
}

/*
<NUMERICAL_EXPRESSION> ::= <NUMERICAL_OPERAND> | <L_PAREN> <NUMERICAL_OPERAND> <R_PAREN> | <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> | <L_PAREN> <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> <R_PAREN> | <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <L_PAREN> <NUMERICAL_EXPRESSION> <R_PAREN> <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> | <L_PAREN> <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> <R_PAREN> | <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <L_PAREN> <NUMERICAL_EXPRESSION> <R_PAREN>
<BOOLEAN_EXPRESSION> ::= <BOOLEAN_TRUE> | <BOOLEAN_FALSE> | <IDENTIFIER> | <IDENTIFIER> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> | <L_PAREN> <IDENTIFIER> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> <R_PAREN> | <NUMERICAL_EXPRESSION> <BOOLEAN_EQUAL> <NUMERICAL_EXPRESSION> | <NUMERICAL_EXPRESSION> <BOOLEAN_NOT_EQUAL> <NUMERICAL_EXPRESSION> | <NUMERICAL_EXPRESSION> <LT_EQUAL> <NUMERICAL_EXPRESSION> | <NUMERICAL_EXPRESSION> <GT_EQUAL> <NUMERICAL_EXPRESSION> | <NUMERICAL_EXPRESSION> <LT> <NUMERICAL_EXPRESSION> | <NUMERICAL_EXPRESSION> <GT> <NUMERICAL_EXPRESSION>
*/

// <EXPRESSION> ::= <BOOLEAN_EXPRESSION> | <NUMERICAL_EXPRESSION>
// currently solving... state = state * 2 + 2;

void RecursiveDescentParser::rExpression(){
    // RULE 1: can be surrounded by Parenthesis
    if(tt == lp){
        sib();
        rExpression();
        if(tt == rp){
            sib();
        }
    }
    else{
        // for now just parse identifiers and integers...
        if(IDis::name(tv)){ // IDENTIFIER
            sib();
        }
        else if(tt == INTEGER){
            sib();
        }
        else if(isNumericalOperator(tt)){ // continue on operators...
            sib();
        }
    }
    if(tt == SEMICOLON || tt == rp){ // lazy rule, terminate on ; inline or ) in for loops
        return;
    }
    else{
        rExpression();
    }
}

void RecursiveDescentParser::rBooleanExpression(){
    /*<BOOLEAN_EXPRESSION> ::= <BOOLEAN_TRUE> |<BOOLEAN_FALSE> | <IDENTIFIER> |
    <IDENTIFIER> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> |
    <L_PAREN> <IDENTIFIER> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> <R_PAREN> |
    <NUMERICAL_EXPRESSION> <BOOLEAN_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <BOOLEAN_NOT_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <LT_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <GT_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <LT> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <GT> <NUMERICAL_EXPRESSION>*/

    // Error in BNF:
    // Added support for arrays in place of IDENTIFIER
    // test4:
    // if (string[i] == '\x0')

    // BNF confusion
    // Have to support
    // if (!empty_string(name))
    // <BOOLEAN NOT><USER_DEFINED_FUNCTION>
    // IDENTIFIER by itself does not support L_PAREN
    // therefor its a UDF which is not linked in properly in BNF

    // RULE 1: can be surrounded by Parenthesis
    if(tt == lp){
        sib();
        throwDebug("inside inner BE");
        rBooleanExpression();
        if(tt == rp){
            sib();
            throwDebug("closed inner BE");
        }
        // Handle two paren enclosed B.E.s test3
        // if ((hex_digit >= '0') && (hex_digit <= '9'))
        if(isBooleanOperator(tt)){ // look for the &&
            sib();
            rBooleanExpression();
        }
    }

    // Since there's nothing about UDFs or boolean_not... let's just handle them at the top level for now
    // let's just put all the logic here and not call any other functions
    // !found_null))
    // !empty_string(name))
    if(tt == BOOLEAN_NOT){
        throwDebug("bn found");
        sib(); // !
        if(tt == id){ // throw an error if the token following the BOOLEAN_NOT isn't an identifier
            sib();
        }
        else{
            std::cerr << "bad boolean expression" << std::endl;
            exit(12);
        }
        if(tt == lp){ sib(); // (id) pattern
            // identifier or identifier list but not required for now... lets just solve for test4
            if(tt == id) sib();
            if(tt == rp) sib();
        }
        // rely on terminal character to return safely...
    }
    // Handle n >= 1 from test1b...
    if(tt == IDENTIFIER){
        if(IDis::name(tv)){
            sib(); // grab IDENTIFIER starting on the left
            // handle arrays
            if(tt == LEFT_BRACKET){
                rIdentifierAndArrayList();
                throwDebug("boolean expression array built");
                rBooleanExpression();
            }
            // BOOLEAN OPERATOR
            if(tt == GT_EQUAL || tt == LT_EQUAL || tt == LT || tt == GT || tt == BOOLEAN_EQUAL || tt == BOOLEAN_NOT_EQUAL){
                sib(); // consume comparison token
                rBooleanExpression(); // recurse
            }
            else if(isNumericalOperator(tt)){
                sib(); // consume comparison token
                throwDebug("inside inner Numerical Operator inside BE");
                rBooleanExpression(); // recurse
            }
        }
    }
    else if(tt == INTEGER){
        sib(); // grab IDENTIFIER starting on the left
        if(tt == GT_EQUAL || tt == LT_EQUAL || tt == LT || tt == GT || tt == BOOLEAN_EQUAL || tt == BOOLEAN_NOT_EQUAL){
            sib(); // consume comparison token
            rBooleanExpression(); // recurse
        }
        else if(isNumericalOperator(tt)){
                sib(); // consume comparison token
                rBooleanExpression(); // recurse
        }
    }
    else if(tt == SINGLE_QUOTE || tt == DOUBLE_QUOTE){
        throwDebug("entering quoted char");
        rSingleDoubleString();
        if(tt == GT_EQUAL || tt == LT_EQUAL || tt == LT || tt == GT || tt == BOOLEAN_EQUAL || tt == BOOLEAN_NOT_EQUAL){
            sib(); // consume comparison token
            rBooleanExpression(); // recurse
        }
        else if(isNumericalOperator(tt)){
                sib(); // consume comparison token
                rBooleanExpression(); // recurse
        }
    }
    else{
        if(tt == GT_EQUAL || tt == LT_EQUAL || tt == LT || tt == GT || tt == BOOLEAN_EQUAL || tt == BOOLEAN_NOT_EQUAL){
            sib(); // consume comparison token
            rBooleanExpression(); // recurse
        }
    }
}
void RecursiveDescentParser::rInitializationExpression(){
    //<INITIALIZATION_EXPRESSION> ::= <IDENTIFIER> <ASSIGNMENT_OPERATOR> <EXPRESSION> | <IDENTIFIER> <ASSIGNMENT_OPERATOR> <SINGLE_QUOTED_STRING> | <IDENTIFIER> <ASSIGNMENT_OPERATOR> <DOUBLE_QUOTED_STRING>
    if(tt == id){
        sib();
        if(tt == ASSIGNMENT_OPERATOR){
            sib();
            if(tt == DOUBLE_QUOTE){
                sib();
                if(tt == STRING){
                    sib();
                    if(tt == DOUBLE_QUOTE){
                        sib();
                        if (tt == SEMICOLON){
                            sib();
                        }
                    }
                }
            }
            else if(tt == SINGLE_QUOTE){
                sib();
                if(tt == STRING){
                    sib();
                    if(tt == SINGLE_QUOTE){
                        sib();
                        if(tt == SEMICOLON){
                            sib();
                        }
                    }
                }
            }
            else{
                rExpression();
            }
        }
    }
}

/*
buildNumericalExpression()
NUMERICAL_EXPRESSION> ::= 
1. <L_PAREN> <NUMERICAL_OPERAND> <R_PAREN> |
2. <L_PAREN> <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> <R_PAREN> |
<L_PAREN> <IDENTIFIER> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> <R_PAREN> |

6. <NUMERICAL_OPERAND> |
5. <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> |
3. <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <L_PAREN> <NUMERICAL_EXPRESSION> <R_PAREN> <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <NUMERICAL_EXPRESSION> |
4. <NUMERICAL_OPERAND> <NUMERICAL_OPERATOR> <L_PAREN> <NUMERICAL_EXPRESSION> <R_PAREN>
*/

/*<BOOLEAN_EXPRESSION> ::= <BOOLEAN_TRUE> |<BOOLEAN_FALSE> | <IDENTIFIER> |
    <IDENTIFIER> <BOOLEAN_OPERATOR> <BOOLEAN_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <BOOLEAN_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <BOOLEAN_NOT_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <LT_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <GT_EQUAL> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <LT> <NUMERICAL_EXPRESSION> |
    <NUMERICAL_EXPRESSION> <GT> <NUMERICAL_EXPRESSION>*/

// HELPERS
bool RecursiveDescentParser::isBooleanOperator(Token_Type tokenType){
    if(tokenType == BOOLEAN_AND || tokenType == BOOLEAN_OR){
        return true;
    }
    return false;
}

bool RecursiveDescentParser::isNumericalOperator(Token_Type tokenType){
    if (tokenType == PLUS || tokenType == MINUS || tokenType == ASTERISK || tokenType == DIVIDE || tokenType == MODULO || tokenType == CARET){
        return true;
    }
    return false;
}

void RecursiveDescentParser::rSingleDoubleString() {
    if (tt == SINGLE_QUOTE) {
        sib();
        while (tt == STRING) {
            sib();
        }
        if (tt == SINGLE_QUOTE) {
            sib();
        }
        else {
            eh.exitUnterminatedStr(lineNumber);
        }
        if (tt == SEMICOLON) {
            sib();
        }
    } else if (tt == DOUBLE_QUOTE) {
        sib();
        while (tt == STRING) {
            sib();
        }
        if (tt == DOUBLE_QUOTE) {
            sib();
        }
        else {
            eh.exitUnterminatedStr(lineNumber);
        }
        if (tt == SEMICOLON) {
            sib();
        }
    }
}
