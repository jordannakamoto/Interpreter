#include "Interpreter.h"

// Requirements:

// integer arithmetic
// sum|n|n|1|+|*|2|n|*|1|+|*|6|/|=

// character - character comparison
//   if ((hex_digit >= '0') && (hex_digit <= '9'))
 
// function call outs
// (sum|sum_of_first_n_squares|(|n|)|=) 
std::string Interpreter::evaluateExpression(){
    
    std::stack<Token*> stack;
    // arithmetic registers, which have to initially be strings to receive values from Tokens
    Token_Type OperatorType;
    std::string a1;
    std::string a2;
    Token r1;
    int lastFilled = 2; // if maybe we need to know which register to fill

    std::string result_message;
    
    std::cout << "\t evaluating expression..." << std::endl;

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
                resultValues.push_back(return_data);

                // & operator creates a pointer
                // so now the operation stack is pointing to items in the AST
                // as well as the result of operations/functions which are stored in
                // Interpreter::resultValues.
                // for clarification, the AST stores Tokens on the heap and resultValues are just normal objects
                // the operation stack uses pointers so we just point to memory in two different locations
                stack.push(&resultValues.back());
            }
            // 2.
            // if the current operator is not a function that needs to be resolved
            // but is a variable IDENTIFIER
            // look for it
            // else{
            //     stack.push(currentStackFrame->getVariable(tokenValue));
            // }
            // else{
            //     throwDebug("Variable not found: " + tokenValue);
            // }
            else{
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
                // one option is to put the logic in here to assume ints only compare to ints
                // and strings only to strings
                // either way we need to convert the value stored in the token as a string
                // with stoi() before doing math on it
            }
            a1 = stack.top()->getTokenValue();  stack.pop();
            a2 = stack.top()->getTokenValue();  stack.pop();
            r1.set_TokenValue("result");
            // just create a dummy result for now and give it some memory
            resultValues.push_back(r1);
            stack.push(&resultValues.back());
            // every time we perform an operation, overwrite the result message
            // so the last operation is recorded
            // for clarification, during an operation, the pc just sits at the operator
            // so we can get it to know the string operation type
            result_message = a2 + " " + pc->getToken()->getTokenValue() + " " + a1;
        }
        // 4.
        else if(tokenType == INTEGER || tokenType == STRING){
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