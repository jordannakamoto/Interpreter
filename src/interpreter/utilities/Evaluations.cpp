#include "Interpreter.h"

// Requirements:

// integer arithmetic
// sum|n|n|1|+|*|2|n|*|1|+|*|6|/|=

// character - character comparison
//   if ((hex_digit >= '0') && (hex_digit <= '9'))
 
// function call outs
// (sum|sum_of_first_n_squares|(|n|)|=) 
Interpreter::IntOrString Interpreter::evaluateExpression(){

    std::stack<Token*> stack;
    // arithmetic registers, which have to initially be strings to receive values from Tokens
    Token_Type OperatorType;
    std::string a1;
    std::string a2;
    std::string r1;
    int lastFilled = 2; // if maybe we need to know which register to fill
    std::cout << "\t evaluating expression..." << std::endl;

    while(true){
        std::string tokenValue = pc->getToken()->getTokenValue();
        Token_Type tokenType = pc->getToken()->getTokenType();
        IntOrString return_data;

        std::cout << pc->getToken()->getTokenValue() << " ";
        // 1.
        // if the current operator is a function it needs to be called and resolved
        if(tokenType == IDENTIFIER && jumpMap.find(tokenValue)){
            std::cout <<  "\n===========\n" << Colors::Magenta  << "Found function callout in expression. Pushing " << tokenValue << "to Call Stack" << Colors::Reset << std::endl;
            // Create a new stack frame for the function
            // Jump to it and run it, awaiting its return value
            pushNewStackFrame(pc->getNextSibling(),pcNum,tokenValue);
            jumpTo(tokenValue);
            return_data = runCall();
            // Store the return value so it can be evaluated as a number or string on the stack (technically int or char)
            // std::get is used to unpack the std::variant dynamic type.
            // Either way the token value is always a string but we can set its type
            if (std::holds_alternative<int>(return_data)) {
                resultValues.push_back(new Token(std::get<std::string>(return_data), INTEGER, -1));
            }
            else{
                resultValues.push_back(new Token(std::get<std::string>(return_data), STRING, -1));
            }
            // ... so basically we're pushing the return value from the callout function
            // onto the postfix stack as a Token since the stack does Token Type eval
            stack.push(resultValues.back());
        }
        // 2.
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
            r1 = "result";
            // just create a dummy result for now and give it some memory
            resultValues.push_back(new Token(r1,INTEGER,-1));
            stack.push(resultValues.back()); 
        }
        // 3.
        else if(tokenType == INTEGER || tokenType == IDENTIFIER || tokenType == STRING){
            // Push the token onto the stack
            stack.push(pc->getToken());
        }
        // All postfix Token Types handled

        // Continue getting the next sibling or break if none
        if(pc->getNextSibling() == nullptr){break;} pc = pc->getNextSibling();
    }
    // No more siblings
    std::cout << Colors::Blue << "\n\t   ...Done with expression eval" << Colors::Reset << std::endl;
    return "";
}

void Interpreter::evaluateIf(){
    
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