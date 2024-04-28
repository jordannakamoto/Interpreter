#include "Interpreter.h"

// Requirements:

// integer arithmetic
// sum|n|n|1|+|*|2|n|*|1|+|*|6|/|=

// character - character comparison
//   if ((hex_digit >= '0') && (hex_digit <= '9'))
 
// function call outs
// (sum|sum_of_first_n_squares|(|n|)|=)

std::string Interpreter::evaluateExpression(){
    int temp1 = -11;
    int temp2 = -11;
    std::stack<int> integerStack;
    STEntry* tempSTEntry = new STEntry();

    std::stack<Token*> stack;
    // arithmetic registers, which have to initially be strings to receive values from Tokens
    Token_Type OperatorType;
    std::string a1;
    std::string a2;
    Token r1;
    int lastFilled = 2; // if maybe we need to know which register to fill
    std::string result_message;
    
    std::cout << "\t evaluating expression... ++" << pc->getToken()->getTokenValue() << std::endl;

    while(true){
        std::string tokenValue = pc->getToken()->getTokenValue();
        Token_Type tokenType = pc->getToken()->getTokenType();

        // Since we return the value of a variable whose memory is stored in a Token
        // The RETURN statement within runCall() will know to just return a pointer
        // to that variable... 
        Token return_data;

        std::cout << pc->getToken()->getTokenValue() << " ";
        // 1.
        // if the current operator is a function it needs to be called and resolved
        if(tokenType == IDENTIFIER){
            // If this Identifier is a function name
            if(jumpMap.find(tokenValue)){
                std::cout <<  "\n===========\n" << Colors::Magenta  << "Found function callout in expression. Pushing " << tokenValue << " to Call Stack" << Colors::Reset << std::endl;
                // Create a new stack frame for the function
                // Jump to it and run it, awaiting its return value
                pushNewStackFrame(pc->getNextSibling(),pcNum,tokenValue);
                jumpTo(tokenValue);
                return_data = runCall();

                //EVERYTHING IN BLOCK COMMENT IS UNNECESSARY JORDAN NAKAMOTO
                /*
                resultValues.push_back(return_data);
                // & operator creates a pointer
                // so now the operation stack is pointing to items in the AST
                // as well as the result of operations/functions which are stored in
                // Interpreter::resultValues.
                // for clarification, the AST stores Tokens on the heap and resultValues are just normal objects
                // the operation stack uses pointers so we just point to memory in two different locations
                stack.push(&resultValues.back());
                */
//                string returned = st->lookupSymbol(return_data.getTokenValue())
                return "Returned from Stack";
            }
            // 2.
            // if the current operator is not a function that needs to be resolved
            // but is a variable IDENTIFIER
            // look for it
            else{
//                stack.push(currentStackFrame->getVariable(tokenValue));
                stack.push(pc->getToken());
            }
        }
        // 3.
        // Is the token an Operator?
        else if(ShuntingYard::isNumericalOperator(tokenType)){
            // In class we pushed the operator onto the working stack I think but
            // we don't need to programatically here since we are directly evaluating
            OperatorType = tokenType;
            // let's pop our two operands ... with some rules
            std::cout << Colors::Black << "\tOperation Instruction" << Colors::Reset << std::endl;

            if(stack.top()->getTokenType() == INTEGER){
                temp1 = stoi(stack.top()->getTokenValue()); stack.pop();
                if(stack.top()->getTokenType() == IDENTIFIER && OperatorType == ASSIGNMENT_OPERATOR){
                    stack.pop();
                    result_message = to_string(temp1);
                    return result_message;
                }
                else if(stack.top()->getTokenType() == IDENTIFIER){
                    a2 = st->lookupSymbol(stack.top()->getTokenValue(), jumpMap.getScopeCount())->getSTValue();
                    temp2 = stoi(a2);
                    stack.pop();
                }
                else{
                    temp2 = stoi(stack.top()->getTokenValue()); stack.pop();
                }
            }
            else if(stack.top()->getTokenType() == IDENTIFIER){
                if(stack.top()->getTokenType() == IDENTIFIER && OperatorType == ASSIGNMENT_OPERATOR){
                    stack.pop();
                    result_message = to_string(temp1);
                    return result_message;
                }
                a1 = st->lookupSymbol(stack.top()->getTokenValue(), jumpMap.getScopeCount())->getSTValue();
                temp1 = stoi(a1);
                stack.pop();

                if(stack.top()->getTokenType() == INTEGER){
                    temp2 = stoi(stack.top()->getTokenValue()); stack.pop();
                }
                else{
                    if(stack.top()->getTokenType() == IDENTIFIER && OperatorType == ASSIGNMENT_OPERATOR){
                        stack.pop();
                        result_message = to_string(temp1);
                        return result_message;
                    }
                    else if(stack.top()->getTokenType() == IDENTIFIER){
                        a2 = st->lookupSymbol(stack.top()->getTokenValue(), jumpMap.getScopeCount())->getSTValue();
                        temp2 = stoi(a2);
                        stack.pop();
                    }
                }
            }

            int result = 0;
            switch (pc->getToken()->getTokenType()) {
                case tdfa::PLUS:
                    result = temp2 + temp1;
                    break;
                case tdfa::MINUS:
                    result = temp2 - temp1;
                    break;
                case tdfa::ASTERISK:
                    result = temp2 * temp1;
                    break;
                case tdfa::DIVIDE:
                    if (temp1 == 0) {
                        // Handle division by zero error
                        throw std::runtime_error("Division by zero");
                    }
                    result = temp2 / temp1;
                    break;
                    // Add more cases for other operators as needed
                default:
                    // Handle unsupported operators
                    throw std::runtime_error("Unsupported operator");
            }

            // Push the result back onto the operand stack
            string tempSTR = to_string(result);
            Token* tempNumPush = new Token(tempSTR, INTEGER, -1);
            stack.push(tempNumPush);

            result_message = tempSTR;
        }
        // 4.
        else if(tokenType == INTEGER){
            // Push the token onto the stack
            stack.push(pc->getToken());
        }
        // All postfix Token Types handled

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


// Notes for possible implementation of evalExpression
// // we can't directly add r1 = a1 + a2 because we don't know their types
            // // way to detect a1 is a string
            // if (std::holds_alternative<std::string>(a1)) {
            //     throwDebug("string at a1");
            //     std::string value = std::get<std::string>(a1);
            // }
            // if (std::holds_alternative<int>(a1)) {
            //     throwDebug("integer at a2");
            //     int value = std::get<int>(a1);
            // }
            /*
            Here's another method to work with variants that chatgpt gave me...
            Alternatively we could actually convert our strings and integers to bytes
            and convert them back after the evaluation step?

            IntOrString add(const IntOrString& v1, const IntOrString& v2) {
                return std::visit([](auto&& arg1, auto&& arg2) -> IntOrString {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    if constexpr (std::is_same_v<T1, int> && std::is_same_v<T2, int>) {
                        // Both are integers
                        return arg1 + arg2;
                    } else {
                        // Convert both to string and concatenate
                        return std::to_string(arg1) + std::to_string(arg2);
                    }
                }, v1, v2);
            }            
            */