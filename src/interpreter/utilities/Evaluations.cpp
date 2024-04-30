#include "Interpreter.h"

// Requirements:

// CASE                              | EXAMPLE
// ----------------------------------|----------------------------------------------
// character to character comparison | if ((hex_digit >= '0') && (hex_digit <= '9'))
// [DONE] function call outs         | (sum|sum_of_first_n_squares|(|n|)|=)
// [DONE] integer arithmetic         | sum|n|n|1|+|*|2|n|*|1|+|*|6|/|=

// Note, we might just want to do a separate evaluation for Boolean expressions
// since I don't think we're doing booleans in ASSIGNMENT operations like x = TRUE even since boolean isn't a data type in the BNF language, it's just int and char

// Numerical Expression
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
                        continue; // loop back to while condition to check paren parity
                    }
                    else{
                        Token* param = pc->getToken();
                        /* case: myfunc(x), need to evaluate variable */
                        if(param->getTokenType() == IDENTIFIER){
                            // see if this parameter is an array
                            AbstractSyntaxTree::Node* lookahead = pc;
                            if((lookahead = pc->getNextSibling())->getToken()->getTokenType() == LEFT_BRACKET){
                                lookahead = lookahead->getNextSibling();
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
                            std::cout << Colors::Magenta << "passing parameter: " << variableValue << Colors::Reset << std::endl;
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
        else if(tokenType != ASSIGNMENT_OPERATOR && ShuntingYard::isNumericalOperator(tokenType)){
            OperatorType = tokenType;
            // Label the step
            std::cout << Colors::Black << "\tOperation Instruction " << Colors::Reset;

            // Fill the registers with the top two stack operands either int or char
            if(stack.top()->getTokenType() == INTEGER){
                temp1 = stoi(stack.top()->getTokenValue());
            }
            else{ // TODO: convert character to a number
            }
            stack.pop();
                
            if(stack.top()->getTokenType() == INTEGER){
                temp2 = stoi(stack.top()->getTokenValue());
            }
            else{ // TODO: convert character to a number
            }
            stack.pop();

            int result = 0;
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
                default:
                    throw std::runtime_error("Unsupported operator");
            }

            // Push the result back onto the operand stack
            string tempSTR = to_string(result);

            /* -- Debug Printing -- */
            std::cout << temp1 << " ";
            if(pc->getToken()->getTokenTypeString() == "ASTERISK"){
                std::cout << "TIMES";
            }
            else{
                std::cout << pc->getToken()->getTokenTypeString();
            }
            std::cout << " " << temp2 << " = " << tempSTR << std::endl;
            /* -------------------- */

            stack.push(new Token(tempSTR, INTEGER, -1));
        }
        // 4. CURRENT NODE is an ORDINARY Integer/Charater
        else if(tokenType == INTEGER){
            // Just push onto stack as an operand
            stack.push(pc->getToken());
            std::cout << Colors::Black <<  "\tPush" << Colors::Reset <<  std::endl;
        }
        // 5. CURRENT NODE is a " quote > determine STRING and push onto stack
        else if(tokenType == DOUBLE_QUOTE){
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

bool Interpreter::evaluateIf(){
    
}

void Interpreter::evaluateForLoop(){

}

void Interpreter::evaluateWhileLoop(){

}
