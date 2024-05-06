#include "Interpreter.h"

// Requirements:

// CASE                              | EXAMPLE                                         | APPEARS IN TEST
// ----------------------------------|------------------------------------------------ | ---------------
// [DONE] function call outs         | (sum|sum_of_first_n_squares|(|n|)|=)            | 1
// [DONE] integer arithmetic         | sum|n|n|1|+|*|2|n|*|1|+|*|6|/|=                 | 1
// [DONE] param passing to callout fn| sum = sum_of_first_n_squares (n)                | 1
// [DONE] char array assignment      | hexnum = "feed\x0";                             | 2
// [DONE] param arr to callout fn    | digit = hexdigit2int (hexnum[i]);               | 2
// ----------------------------------|------------------------------------------------ | ---------------
// character to character comparison | if ((hex_digit >= '0') && (hex_digit <= '9'))   | 2


// NOTE: test cases don't contain compound boolean/numerical expressions so we can handle them separately

// Evaluate Assignment - handles Numerical
// TODO: rename
std::string Interpreter::evaluateExpression(){
    // registers
    int temp1 = -11;
    int temp2 = -11;

    std::stack<Token*> stack;
    Token_Type OperatorType;
    std::string result_message;
    
    std::cout << "\t evaluating expression... " << pc->getToken()->getTokenValue() << std::endl;

    // Important:
    // the first sibling... is the variable that the expression ends up assigning to
    std::string varName = pc->getToken()->getTokenValue();

    pc = pc->getNextSibling();

    while(true){
        std::string tokenValue = pc->getToken()->getTokenValue();
        Token_Type tokenType = pc->getToken()->getTokenType();
        std::cout << tokenValue << " " << tokenType << std::endl;

        std::cout << pc->getToken()->getTokenValue() << " ";

        // 1.
        // CURRENT NODE is a FUNCTION
        if(tokenType == IDENTIFIER){
            
            // called function and push its return value onto stack
            if(jumpMap.find(tokenValue)){
                std::cout <<  "\n===========\n" << Colors::Magenta  << "Found function callout in expression. Pushing " << tokenValue << " to Call Stack" << Colors::Reset << std::endl;
            
                // Grab arguments before calling
                pc = pc->getNextSibling(); // consume L_PAREN
                int parenCounter = 1;      
                pc = pc->getNextSibling(); // set pc at first argument
                std::vector<std::string> arguments;

                // gather all arguments within param list
                while(parenCounter != 0){
                    Token_Type argumentToken = pc->getToken()->getTokenType();
                    if(argumentToken == LEFT_PARENTHESIS){
                        parenCounter++;
                    }
                    else if(argumentToken == RIGHT_PARENTHESIS){
                        parenCounter--;
                        continue; // loop back to while condition to break on paren parity
                    }
                    else{
                        Token* param = pc->getToken();
                        /* case: myfunc(x), need to evaluate variable */
                        if(param->getTokenType() == IDENTIFIER){
                            // see if this parameter is an array
                            AbstractSyntaxTree::Node* lookahead = pc;
                            if((pc->getNextSibling())->getToken()->getTokenType() == LEFT_BRACKET){
                                lookahead = lookahead->getNextSibling()->getNextSibling();
                                pc = lookahead->getNextSibling(); // exit the array syntax
                            }
                            std::string variableValue;
                            if(lookahead !=pc){ // if we've performed the array parsing
                                int accessIndex;
                                // resolve the index accessor if it's a variable in the current scope
                                if(lookahead->getToken()->getTokenType() == IDENTIFIER){
                                    accessIndex = stoi(currentStackFrame->getVariable(lookahead->getToken()->getTokenValue())->getTokenValue());
                                }
                                // otherwise its just a raw index int
                                else{ 
                                    accessIndex = stoi(lookahead->getToken()->getTokenValue());
                                }
                                variableValue = currentStackFrame->getVarArray(param->getTokenValue(),accessIndex)->getTokenValue();
                            }
                            else{
                                variableValue = currentStackFrame->getVariable(param->getTokenValue())->getTokenValue();
                            }
                            // if the parameter is a variable in the current scope, resolve it before passing as a parameter to the callout
                            // std::cout << Colors::Black << "passing parameter... " << variableValue << Colors::Reset << std::endl;
                            arguments.push_back(variableValue);
                        }
                        /* otherwise its just a normal value - i.e. myfunc(5) */
                        else{
                            arguments.push_back(param->getTokenValue());
                        }
                    }
                    pc = pc->getNextSibling();
                }
                // Now that params are gathered,
                // Jump to function and run it
                pushNewStackFrame(pc,pcNum,tokenValue);
                for(int i = 0; i < arguments.size();i++){
                    currentStackFrame->setParameter(i, arguments[i]);
                }
                printCurrentStackFrame();
                jumpTo(tokenValue);
                Token return_data = runCall();

                stack.push(new Token(return_data));
            }

        // 2. CURRENT NODE is a VARIABLE
            else{
               // evaluate it and put it on the stack
               Token* storedVariable = currentStackFrame->getVariable(tokenValue);
               stack.push(storedVariable);
               std::cout << Colors::Black << "\tPush " << Colors::Reset <<  storedVariable->getTokenValue() << std::endl;
            }
        }
        // 3. CURRENT NODE is an OPERATOR besides the final assignment op
        else if(tokenType != ASSIGNMENT_OPERATOR && (ShuntingYard::isNumericalOperator(tokenType) || ShuntingYard::isBooleanOperator(tokenType))){
            OperatorType = tokenType;
            // Label the step
            std::cout << Colors::Black << "\tOperation Instruction " << Colors::Reset;

            // Fill the registers with the top two stack operands either int or char
            // temp1
            if(stack.top()->getTokenType() == INTEGER){
                std::cout << Colors::Red << "FOUND AN INTEGER" << Colors::Reset << std::endl; // Debug
                temp1 = stoi(stack.top()->getTokenValue());
            }
            else if(stack.top()->getTokenType() == CHARACTER || stack.top()->getTokenType() == STRING){
                // get the first character in the tokenValue... will work for either char or string
                // TODO: String arithmetic
                char firstChar = stack.top()->getTokenValue()[0];
                temp1 = static_cast<int>(firstChar); 
                std::cout << Colors::Red << "CHAR IS " << temp1 << Colors::Reset << std::endl; // DEBUG
            }
            else{
                __throw_runtime_error("unexpected token type on operand stack");
            }
            stack.pop();
            
            // temp2
            if(stack.top()->getTokenType() == INTEGER){
                temp2 = stoi(stack.top()->getTokenValue());
            }
            else if(stack.top()->getTokenType() == CHARACTER || stack.top()->getTokenType() == STRING){
                // get the first character in the tokenValue... will work for either char or string
                // TODO: String arithmetic
                char firstChar = stack.top()->getTokenValue()[0];
                temp2 = static_cast<int>(firstChar);
                std::cout << Colors::Red << "CHAR IS " << temp2 << Colors::Reset << std::endl; // DEBUG
            }
            else{
                __throw_runtime_error("unexpected token type on operand stack");
            }
            stack.pop();

            int result = 0;
            std::cout << OperatorType << std::endl; // DEBUG
            if (OperatorType == Token_Type::BOOLEAN_OR) {
                std::cout << "WHAAAA"<< std::endl; // DEBUG
            }
            switch (OperatorType) {
                case PLUS:
                    result = temp2 + temp1;
                    break;
                case MINUS:
                    result = temp2 - temp1;
                    break;
                case ASTERISK:
                    result = temp2 * temp1;
                    break;
                case DIVIDE:
                    if (temp1 == 0 || temp2 == 0) {
                        throw std::runtime_error("Division by zero"); // Handle division by zero error
                    }
                    result = temp2 / temp1;
                    break;
                case MODULO:
                    result = temp2 % temp1;
                    break;
                case BOOLEAN_OR:
                    if (temp1 || temp2) {
                        result = true;
                    }
                    else result = false;
                    break;
                case BOOLEAN_AND:
                    if (temp1 && temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case BOOLEAN_EQUAL:
                    if (temp1 == temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case BOOLEAN_NOT_EQUAL:
                    if (temp1 != temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                    // REMEMBER temp1 is actually operator 2, and temp2 is operator 1
                case LT:
                    if (temp1 > temp2) {
                        result = true;
                    }
                    else {
                        std::cout << "true" << Colors::Reset << std::endl;
                        result = false;
                    }
                    break;
                case GT:
                    if (temp1 < temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case LT_EQUAL:
                    if (temp1 >= temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case GT_EQUAL:
                    if (temp1 <= temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                default:
                    throw std::runtime_error("Unsupported operator");
            }
            std::cout << "\tresult " << Colors::Red << result <<Colors::Reset<< std::endl; // DEBUG

            // Push the result back onto the operand stack
            string tempSTR = to_string(result);

            /* -- Debug Printing -- */
            std::cout << temp2 << " ";
            if(pc->getToken()->getTokenTypeString() == "ASTERISK"){
                std::cout << "TIMES";
            }
            else{
                std::cout << pc->getToken()->getTokenTypeString();
            }
            std::cout << " " << temp1 << " = " << tempSTR << std::endl;
            /* -------------------- */

            stack.push(new Token(tempSTR, INTEGER, -1));
        }
        // 4. CURRENT NODE is an ORDINARY Integer/Charater
        else if(tokenType == INTEGER){
            // Just push onto stack as an operand
            stack.push(pc->getToken());
            std::cout << Colors::Black <<  "\tPush" << Colors::Reset <<  std::endl;
        }
        // 5. CURRENT NODE is a " or ' quote : grab STRING and push onto stack
        else if(tokenType == DOUBLE_QUOTE || tokenType == SINGLE_QUOTE){
            pc = pc->getNextSibling(); // get string within " "
            if(pc->getToken()->getTokenType() == STRING){
                stack.push(pc->getToken());
            }
            else{
                throw std::runtime_error("String not found within double quotes");
            }
            pc = pc->getNextSibling(); // exit the quotation
        }
        // 6. CURRENT NODE is the final ASSIGNMENT OPERATOR
        else if(tokenType == ASSIGNMENT_OPERATOR){
            // Complete the variable assignment
            std::string assignmentValue = stack.top()->getTokenValue();
            stack.pop();
            std::cout << varName << std::endl;
            
            if(currentStackFrame->getVariable(varName)){
                currentStackFrame->setVariable(varName, assignmentValue);
            }
            else if(currentStackFrame->getVarArray(varName,0)){ // check for existence if there's an element at index 0
                currentStackFrame->setArrayVariableFromString(varName, assignmentValue);
            }

            result_message += assignmentValue + " assigned to " + varName;
        }
        
        // ... Traversal
        // Continue getting the next sibling or break if none
        if(pc->getNextSibling() == nullptr){break;} pc = pc->getNextSibling();
    }
    // No more siblings
    std::cout << Colors::Blue << "\n\t   ...Done with expression eval" << Colors::Reset << std::endl;
    return result_message;
}

bool Interpreter::evaluateBoolCondition() {

    std::stack<Token*> stack;
    Token_Type OperatorType;
    std::string result_message;
    int temp1 = -11;
    int temp2 = -11;
    int result = 0;
    
    std::cout << "\t evaluating expression... " << pc->getToken()->getTokenValue() << std::endl;

    // Important:
    // the first sibling... is the variable that the expression ends up assigning to
    std::string varName = pc->getToken()->getTokenValue();

    pc = pc->getNextSibling(); // DEBUG

    while(true){
        std::string tokenValue = pc->getToken()->getTokenValue();
        Token_Type tokenType = pc->getToken()->getTokenType();

        std::cout << pc->getToken()->getTokenValue() << " ";

        // 1.
        // CURRENT NODE is a FUNCTION
        if(tokenType == IDENTIFIER){
            // called function and push its return value onto stack
            if(jumpMap.find(tokenValue)){
                std::cout <<  "\n===========\n" << Colors::Magenta  << "Found function callout in expression. Pushing " << tokenValue << " to Call Stack" << Colors::Reset << std::endl;
            
                // Grab arguments before calling
                pc = pc->getNextSibling(); // consume L_PAREN
                int parenCounter = 1;      
                pc = pc->getNextSibling(); // set pc at first argument
                std::vector<std::string> arguments;

                // gather all arguments within param list
                while(parenCounter != 0){
                    Token_Type argumentToken = pc->getToken()->getTokenType();
                    if(argumentToken == LEFT_PARENTHESIS){
                        parenCounter++;
                    }
                    else if(argumentToken == RIGHT_PARENTHESIS){
                        parenCounter--;
                        continue; // loop back to while condition to break on paren parity
                    }
                    else{
                        Token* param = pc->getToken();
                        /* case: myfunc(x), need to evaluate variable */
                        if(param->getTokenType() == IDENTIFIER){
                            // see if this parameter is an array
                            AbstractSyntaxTree::Node* lookahead = pc;
                            if((pc->getNextSibling())->getToken()->getTokenType() == LEFT_BRACKET){
                                lookahead = lookahead->getNextSibling()->getNextSibling();
                                pc = lookahead->getNextSibling(); // exit the array syntax
                            }
                            std::string variableValue;
                            if(lookahead !=pc){ // if we've performed the array parsing
                                int accessIndex;
                                // resolve the index accessor if it's a variable in the current scope
                                if(lookahead->getToken()->getTokenType() == IDENTIFIER){
                                    accessIndex = stoi(currentStackFrame->getVariable(lookahead->getToken()->getTokenValue())->getTokenValue());
                                }
                                // otherwise its just a raw index int
                                else{ 
                                    accessIndex = stoi(lookahead->getToken()->getTokenValue());
                                }
                                variableValue = currentStackFrame->getVarArray(param->getTokenValue(),accessIndex)->getTokenValue();
                            }
                            else{
                                variableValue = currentStackFrame->getVariable(param->getTokenValue())->getTokenValue();
                            }
                            // if the parameter is a variable in the current scope, resolve it before passing as a parameter to the callout
                            // std::cout << Colors::Black << "passing parameter... " << variableValue << Colors::Reset << std::endl;
                            arguments.push_back(variableValue);
                        }
                        /* otherwise its just a normal value - i.e. myfunc(5) */
                        else{
                            arguments.push_back(param->getTokenValue());
                        }
                    }
                    pc = pc->getNextSibling();
                }
                // Now that params are gathered,
                // Jump to function and run it
                pushNewStackFrame(pc,pcNum,tokenValue);
                for(int i = 0; i < arguments.size();i++){
                    currentStackFrame->setParameter(i, arguments[i]);
                }
                printCurrentStackFrame();
                jumpTo(tokenValue);
                Token return_data = runCall();

                stack.push(new Token(return_data));
            }

        // 2. CURRENT NODE is a VARIABLE
            else{
               // evaluate it and put it on the stack
               Token* storedVariable = currentStackFrame->getVariable(tokenValue);
               stack.push(storedVariable);
               std::cout << Colors::Black << "\tPush " << Colors::Reset <<  storedVariable->getTokenValue() << std::endl;
            }
        }
        // 3. CURRENT NODE is an OPERATOR besides the final assignment op
        else if(tokenType != ASSIGNMENT_OPERATOR && (ShuntingYard::isNumericalOperator(tokenType) || ShuntingYard::isBooleanOperator(tokenType))){
            OperatorType = tokenType;
            // Label the step
            /*
            std::cout << stack.top()->getTokenType() << std::endl; // DEBUG
            std::cout << stack.top()->getTokenValue() << std::endl; // DEBUG
            */

            // Fill the registers with the top two stack operands either int or char
            // temp1
            if(stack.top()->getTokenType() == INTEGER){
                temp1 = stoi(stack.top()->getTokenValue());
            }
            else if(stack.top()->getTokenType() == CHARACTER || stack.top()->getTokenType() == STRING){
                // get the first character in the tokenValue... will work for either char or string
                // TODO: String arithmetic
                char firstChar = stack.top()->getTokenValue()[0];
                temp1 = static_cast<int>(firstChar);
            }
            else{
                __throw_runtime_error("unexpected token type on operand stack");
            }
            stack.pop();
            
            // temp2
            /*
            std::cout << stack.top()->getTokenType() << std::endl; // DEBUG
            std::cout << stack.top()->getTokenValue() << std::endl; // DEBUG
            */
            if(stack.top()->getTokenType() == INTEGER){
                temp2 = stoi(stack.top()->getTokenValue());
            }
            else if(stack.top()->getTokenType() == CHARACTER || stack.top()->getTokenType() == STRING){
                // get the first character in the tokenValue... will work for either char or string
                // TODO: String arithmetic
                char firstChar = stack.top()->getTokenValue()[0];
                temp2 = static_cast<int>(firstChar);
            }
            else{
                __throw_runtime_error("unexpected token type on operand stack");
            }
            stack.pop();

            std::cout << OperatorType << std::endl; // DEBUG
            if (OperatorType == Token_Type::BOOLEAN_OR) {
                std::cout << "WHAAAA"<< std::endl; // DEBUG
            }
            switch (OperatorType) {
                case PLUS:
                    result = temp2 + temp1;
                    break;
                case MINUS:
                    result = temp2 - temp1;
                    break;
                case ASTERISK:
                    result = temp2 * temp1;
                    break;
                case DIVIDE:
                    if (temp1 == 0 || temp2 == 0) {
                        throw std::runtime_error("Division by zero"); // Handle division by zero error
                    }
                    result = temp2 / temp1;
                    break;
                case MODULO:
                    result = temp2 % temp1;
                    break;
                case BOOLEAN_OR:
                    if (temp1 || temp2) {
                        result = true;
                    }
                    else result = false;
                    break;
                case BOOLEAN_AND:
                    if (temp1 && temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case BOOLEAN_EQUAL:
                    if (temp1 == temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case BOOLEAN_NOT_EQUAL:
                    if (temp1 != temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                    // REMEMBER temp1 is actually operator 2, and temp2 is operator 1
                case LT:
                    if (temp1 > temp2) {
                        result = true;
                    }
                    else {
                        std::cout << "true" << Colors::Reset << std::endl;
                        result = false;
                    }
                    break;
                case GT:
                    if (temp1 < temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case LT_EQUAL:
                    if (temp1 >= temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                case GT_EQUAL:
                    if (temp1 <= temp2) {
                        result = true;
                    }
                    else {
                        result = false;
                    }
                    break;
                default:
                    throw std::runtime_error("Unsupported operator");
            }
            std::cout << "\tresult " << Colors::Red << result <<Colors::Reset<< std::endl; // DEBUG

            // Push the result back onto the operand stack
            string tempSTR = to_string(result);

            stack.push(new Token(tempSTR, INTEGER, -1));
        }
        // 4. CURRENT NODE is an ORDINARY Integer/Charater
        else if(tokenType == INTEGER){
            // Just push onto stack as an operand
            stack.push(pc->getToken());
            std::cout << Colors::Black <<  "\tPush" << Colors::Reset <<  std::endl;
        }
        // 5. CURRENT NODE is a " or ' quote : grab STRING and push onto stack
        else if(tokenType == DOUBLE_QUOTE || tokenType == SINGLE_QUOTE){
            pc = pc->getNextSibling(); // get string within " "
            if(pc->getToken()->getTokenType() == STRING){
                stack.push(pc->getToken());
            }
            else{
                throw std::runtime_error("String not found within double quotes");
            }
            pc = pc->getNextSibling(); // exit the quotation
        }
        
        // ... Traversal
        // Continue getting the next sibling or break if none
        if(pc->getNextSibling() == nullptr){break;} pc = pc->getNextSibling();
    }
    // No more siblings
    std::cout << Colors::Blue << "\n\t   ...Done with BOOLEAN eval: " << result << Colors::Reset << std::endl;
    if (result < 0 || result > 1) {
        std::cout << "BAD RESULT " << result << std::endl;
        throw std::runtime_error("Non boolean evaluation for condition");
    }
    return result;
}

bool Interpreter::evaluateIf(){
    
}

void Interpreter::evaluateForLoop(){
    std::cout << "\tinitializing loop variable" << std::endl;
    processForAssignment();
    if (pc->getNextChild() == nullptr) {
        throw std::runtime_error("for loop needs 3 expressions");
    }
    pc = pc->getNextChild();

    auto forCondition = pc;
    std::cout << Colors::Cyan << "\tevaluating for condition" << std::endl;
    bool condition = evaluateBoolCondition();
    if (pc->getNextChild() == nullptr) {
        throw std::runtime_error("for loop needs 3 expressions");
    }
    pc = pc->getNextChild();

    auto itrExpression = pc;
    // iterate to end of sibling branch to get to next statement
    while(pc->getNextSibling() != nullptr) {
        pc = pc->getNextSibling();
    }
    pc = pc->getNextChild();

    // should be at begin block
    if (pc->getToken()->getTokenType() != AST_BEGIN_BLOCK) {
        throw std::runtime_error("required begin block after for loop");
    }

    pc = pc->getNextChild();

    auto startBlock = pc;

    do {
        int braceCounter = 1;
        // go through brace block of for loop
        while (braceCounter >= 1)
        {
            // For tracking parity of {} in IF/ELSE groups
            auto tokenType = pc->getToken()->getTokenType();
            std::cout << pc->getToken()->getTokenValue() << std::endl;
            switch (tokenType)
            {
                case AST_BEGIN_BLOCK:
                    braceCounter++;
                    break;
                case AST_END_BLOCK:
                    braceCounter--;
                    // if we're in global space, we're not looking for an end block
                    // global termination handled below in Traverse line 149 with break statement at AST end
                    // the print statement above on line 88 will still execute tho to indicate where pc is
                    break;
                case AST_ASSIGNMENT:
                    std::cout << "\t> TODO: parse and evaluate an assignment" << std::endl;
                    processAssignment();
                    break;
                case AST_CALL:
                    // callStack.push
                    std::cout << "\t> TODO: parse and evaluate a call" << std::endl;
                    break;
                case AST_IF:
                    processIfStatement(); // Probably need to pass scopeBlockStack
                    break;
                case AST_FOR:
                    std::cout << "\t> TODO: parse and evaluate a for loop" << std::endl;
                    processForLoop();
                    break;
                case AST_WHILE:
                    std::cout << "\t> TODO: parse and evaluate a while loop" << std::endl;
                    // Similar to for loops
                    processWhileLoop();
                    break;
                case AST_RETURN:
                    processReturnStatement();
                    break;
                case AST_PRINTF:
                    processPrintStatement();
                    break;
                case AST_ELSE:
                    if (!processElse) {
                        jumpToScopeEnd();
                    }
                    break;
                default:
                    break;
                };
            
            // ... Traverse
            if (pc->getNextSibling() == nullptr)
            {
                if (pc->getNextChild() != nullptr)
                {
                    pc = pc->getNextChild();
                    pcNum++; // pcNum at least increases with every child of the AST
                }
                else{ // Terminate call if we're at end of AST ~ end of program
                    break;
                }
            }
            else
            {
                pc = pc->getNextSibling();
            }
        }

        // perform the post loop iteration expression
        pc = itrExpression;
        std::cout << Colors::Cyan << "\tevaluating itr expression ";
        processAssignment();

        // evaluate loop condition
        pc = forCondition;
        std::cout << "\tevaluating for condition "; // DEBUG
        condition = evaluateBoolCondition();
        std::cout << condition << std::endl; // DEBUG
        
        pc = startBlock;
    } while (condition);
    // the problem is here DEBUG
    // 
    int braceCounter = 1;
    std::cout << "jumping to end of for loop" << std::endl; // DEBUG
    while (braceCounter >= 1) {
        if (pc ->getToken()->getTokenType() == AST_END_BLOCK) {
            braceCounter--;
        }
        else if (pc->getToken()->getTokenType() == AST_BEGIN_BLOCK) {
            braceCounter++;
        }
        if (pc->getNextSibling() != nullptr) {
            pc = pc->getNextSibling();
        }
        else if (pc->getNextChild() != nullptr){
            pc = pc->getNextChild();
        }
        else {
            throw std::runtime_error("something wrong with ending for loop");
        }
    }
    std::cout << Colors::Reset;
}

void Interpreter::evaluateWhileLoop(){
    auto loopCondition = pc;
    bool condition = evaluateBoolCondition();
    int braceCounter;
    while(condition) {
        pc = pc->getNextChild();

        if (pc->getToken()->getTokenType() != AST_BEGIN_BLOCK) {
            throw std::runtime_error("something wrong with ending for loop");
        }
        
        braceCounter = 1;
        while (braceCounter >= 1)
        {
            // For tracking parity of {} in IF/ELSE groups
            auto tokenType = pc->getToken()->getTokenType();
            std::cout << pc->getToken()->getTokenValue() << std::endl;
            switch (tokenType)
            {
                case AST_BEGIN_BLOCK:
                    braceCounter++;
                    break;
                case AST_END_BLOCK:
                    braceCounter--;
                    // if we're in global space, we're not looking for an end block
                    // global termination handled below in Traverse line 149 with break statement at AST end
                    // the print statement above on line 88 will still execute tho to indicate where pc is
                    break;
                case AST_ASSIGNMENT:
                    std::cout << "\t> TODO: parse and evaluate an assignment" << std::endl;
                    processAssignment();
                    break;
                case AST_CALL:
                    std::cout << "\t> TODO: parse and evaluate a call" << std::endl;
                    processCallStatement();
                    break;
                case AST_IF:
                    processIfStatement(); // Probably need to pass scopeBlockStack
                    break;
                case AST_FOR:
                    std::cout << "\t> TODO: parse and evaluate a for loop" << std::endl;
                    processForLoop();
                    break;
                case AST_WHILE:
                    std::cout << "\t> TODO: parse and evaluate a while loop" << std::endl;
                    // Similar to for loops
                    processWhileLoop();
                    break;
                case AST_RETURN:
                    processReturnStatement();
                    break;
                case AST_PRINTF:
                    processPrintStatement();
                    break;
                case AST_ELSE:
                    if (!processElse) {
                        jumpToScopeEnd();
                    }
                    break;
                default:
                    break;
                };
            
            // ... Traverse
            if (pc->getNextSibling() == nullptr)
            {
                if (pc->getNextChild() != nullptr)
                {
                    pc = pc->getNextChild();
                    pcNum++; // pcNum at least increases with every child of the AST
                }
                else{ // Terminate call if we're at end of AST ~ end of program
                    break;
                }
            }
            else
            {
                pc = pc->getNextSibling();
            }
        }
        pc = loopCondition;
        condition = evaluateBoolCondition();
    }
    
    pc = pc->getNextChild();
    if (pc->getToken()->getTokenType() != AST_BEGIN_BLOCK) {
        std::cerr << "problems getting first brace after while loop" << std::endl;
        throw(-1);
    }

    pc = pc->getNextChild();

    
    // jump to section after loop when condition fails
    braceCounter = 1;
    std::cout << "jumping to end of while loop" << std::endl; // DEBUG
    while (braceCounter >= 1) {
        if (pc ->getToken()->getTokenType() == AST_END_BLOCK) {
            braceCounter--;
        }
        else if (pc->getToken()->getTokenType() == AST_BEGIN_BLOCK) {
            braceCounter++;
        }
        if (pc->getNextSibling() != nullptr) {
            pc = pc->getNextSibling();
        }
        else if (pc->getNextChild() != nullptr){
            pc = pc->getNextChild();
        }
        else {
            throw std::runtime_error("something wrong with ending of while loop");
        }
    }
}
