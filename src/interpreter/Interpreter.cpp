#include "Interpreter.h"

// Constructor
// Note about Program Counter:
// pc and pcNum represent the same idea
// A Marker to where we are in the program's execution

// pc    - Program Counter is the pointer to an AST Node
// pcNum - is a numbered count
Interpreter::Interpreter(SymbolTable* _st, AbstractSyntaxTree& _ast): st(_st), ast(_ast), pc(nullptr), pcNum(1){}

// ---------------------------------------------------------------- //
// PREPROCESS
// 1. Activate the CallStack by pushing Global StackFrame
// ... this initializes global variables by getting them from SymbolTable
// 2. Scan for functions/procedures to fill the JumpMap
// ---------------------------------------------------------------- //
void Interpreter::preprocess(){

    pushNewGlobalStackFrame(); // push global stack frame
    jumpMap = JumpMap(st);     // link JumpMap to SymbolTable

    // jumpMap.add(astNode*, PC#) creates an Entry w/ Program Counter marker information
    // then connects it to the right function from the symbol table

    AbstractSyntaxTree::Node* curr = ast.head;
    Token_Type tt;
    while(curr != nullptr){
        tt = curr->getToken()->getTokenType();
        if(tt == AST_FUNCTION_DECLARATION || tt == AST_PROCEDURE_DECLARATION){
            jumpMap.add(curr->getNextChild()->getNextChild(),pcNum); // > puts the PC marker after the BEGIN BLOCK
        }
        // Traverse...
        if(curr->getNextSibling() == nullptr){
            curr = curr->getNextChild();
            pcNum++;
        }
        else{
            // Numerical PC theoretically also increases for siblings that represent a program Instruction.
            // but this doesn't actually matter for us unless we want to do a deeper simulation of low level stuff
            curr = curr->getNextSibling();
        }
    }
    pc_END = pcNum - 1; // Store the end of program# for debug or something
};

void Interpreter::run(){
    // init AST ProgramCounter
    // jump to main from the JumpMap
    // enter main loop which executes until all calls on the stack have returned
    preprocess();
    pc = ast.head;
    jumpTo("main");
    pushNewStackFrame(nullptr,0, "main"); // push main, which like global, doesn't jump the PC anywhere when it returns
    while(!callStack.empty()){
        runCall();
    }
    std::cout << "program run complete" << std::endl;
}
// ---------------------------------------------------------------- //
// RUN CALL
// ---------------------------------------------------------------- //
/**
 * Runs a function call in the interpreter.
 *
 * This method is responsible for executing the logic of a function call in the interpreter. It handles the control flow of the function call, including pushing and popping the call stack, tracking the current program counter, and returning the result of the function call.
 *
 * @return The result of the function call, represented as an `IntOrString` union type.
 */
/**
 * Runs a function call in the interpreter.
 *
 * This method is responsible for executing the logic of a function call, including
 * handling the scope of the call, processing different types of statements (assignment,
 * if, for, while), and managing the call stack.
 *
 * The method uses a stack to track the scope of the function call, and processes
 * different types of statements based on their token type. It also handles the
 * return from the function call, updating the program counter and call stack
 * accordingly.
 *
 * @return The return value of the function call, as an IntOrString object.
 */
Interpreter::IntOrString Interpreter::runCall()
{

    Token_Type tokenType;
    IntOrString returnValue;

    std::stack<char> scopeBlockStack;
    scopeBlockStack.push('{'); // start after the BEGIN_BLOCK
    while (!scopeBlockStack.empty())
    {
        // For tracking parity of {} in IF/ELSE groups
        tokenType = pc->getToken()->getTokenType();
        std:cout << pc->getToken()->getTokenValue() << std::endl;
        switch (tokenType)
        {
        case AST_BEGIN_BLOCK:
            scopeBlockStack.push('{');
            std::cout << "\t+ pushed on { scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
            break;
        case AST_END_BLOCK:
            scopeBlockStack.pop();
            std::cout << "\t- popped on } scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
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
            std::cout << "\t> TODO: parse and evaluate an if condition" << std::endl;
            processIfStatement(); // Probably need to pass scopeBlockStack
            // Note: Handling the complexity of the IF/ELSE statement block handling is a significant feature
            // In a program that spends lots of cycles in the same if/else statements
            // or needs to run for a long time or perhaps has large if blocks
            // adding jump markers during preprocessing would probably be wise
            break;
        case AST_FOR:
            std::cout << "\t> TODO: parse and evaluate a for loop" << std::endl;
            // Note: Furthermore the complexity of for loops recursing back into Ifs is unclear to me presently
            // We could either place them on the call stack or create some struct for them
            // to store their state. There aren't any nested for loops in the tests but we would at least need
            // a vector of whatever struct or data fields they require.
            processForLoop();
            break;
        case AST_WHILE:
            std::cout << "\t> TODO: parse and evaluate a while loop" << std::endl;
            // Similar to while loops
            processWhileLoop();
            break;
        default:
            break;
        };
        // Once again, PC is the marker pointer to the ast
        // like we've called astHead before but if Prof Bruce reads our code
        // he might get some delight
        if (pc->getNextSibling() == nullptr)
        {
            // pcNum at least increases with every child of the AST
            if (pc->getNextChild() != nullptr)
            {
                pc = pc->getNextChild();
                pcNum++;
            }
        }
        else
        {
            pc = pc->getNextSibling();
        }
    };
    // * RETURN Routine * //
    // Handle returning from a normal call or a special exit from main
    if (currentStackFrame->returnPCNum > 0)
    {
        std::cout << "-------------------\n"
                  << Colors::Magenta << "Returning from call... back to PC: " << Colors::Reset << currentStackFrame->returnPCNum << "\n===================" << std::endl;
    }
    else if (currentStackFrame->returnPCNum == 0)
    {
        std::cout << "-------------------\n"
                  << Colors::Magenta << "Returning from main: " << Colors::Reset << std::endl;
    }
    if (currentStackFrame->returnPC != nullptr)
    {
        pc = currentStackFrame->returnPC; // move PC to the return destination if there is one
    }
    callStack.pop_back(); // pop the call off the stack when its done
    currentStackFrame = &callStack.back();
    // throwDebug("RETURNING FROM CALL");
    return "result"; // TODO: return something based on a variable specified in the return statement
};

// ** Helpers before Eval ** //
// the main run switch-case goes here first before eval
// in case we need any special handling or perhaps don't want to do the implementation in the eval file

void Interpreter::processAssignment(){
    pc = pc->getNextSibling();
    evaluateExpression();
}

void Interpreter::processIfStatement(){
    pc = pc->getNextSibling();
    evaluateIf();
}

void Interpreter::processForLoop(){
    pc = pc->getNextSibling();
    evaluateForLoop();
}

void Interpreter::processWhileLoop(){
    pc = pc->getNextSibling();
    evaluateWhileLoop();
}
/* ----------------------------------------------------- */
/* METHODS                                               */
/* ----------------------------------------------------- */

// JumpTo
// Moves the PC pointer to a function pointer in the map by its name
void Interpreter::jumpTo(std::string name){
    std::cout << Colors::Green << "jumping to..." << name << "*" << Colors::Reset << std::endl;
    pc = jumpMap.getPC(name);
    pcNum = jumpMap.getPCNum(name);
}

// PushNewStackFrame
void Interpreter::pushNewStackFrame(AbstractSyntaxTree::Node*pc, int pcNum,std::string name){
    StackFrame temp;
    temp.returnPC = pc;
    temp.returnPCNum = pcNum;
    int function_scope = jumpMap.getScope(name);
    temp.stEntry = st->searchSymbolTableByScope(function_scope); // this isn't used anywhere yet
    // but this is...
    std::vector<STEntry*> function_variables = st->getVariablesByScope(function_scope);
    for(STEntry* entry : function_variables){
        if(entry->getD_Type() == d_int){
            temp.setVariable(entry->getIDName(), 0);
        }
        else if(entry->getD_Type() == d_char){
            temp.setVariable(entry->getIDName(), "");
        }
    }
    callStack.push_back(temp);
    currentStackFrame = &callStack.back();
    printCurrentStackFrame();
}

// PushNewStackFrame without symbol table handling
// simpler version for the global frame
void Interpreter::pushNewGlobalStackFrame(){
    StackFrame new_global_frame;
    new_global_frame.returnPC = nullptr;
    new_global_frame.returnPCNum = -1;   // returns to null AST Node and -1 for now to indicate no return
    
    std::vector<STEntry*> results = st->getVariablesByScope(0);
    for(STEntry* entry : results){
        // Initialize
        // Integers:0 , Strings: ""
        if(entry->getD_Type() == d_int){
            new_global_frame.setVariable(entry->getIDName(), 0);
        }
        else if(entry->getD_Type() == d_char){
            new_global_frame.setVariable(entry->getIDName(), "");
        }
    }

    callStack.push_back(new_global_frame);
    currentStackFrame = &callStack.back(); // update the variable storing the current frame
    printCurrentStackFrame();

}

// Throw a debug message to print
    
void Interpreter::throwDebug(std::string msg){
    std::cout << Colors::Red << msg << std::endl;
    std::cout << pc->getToken()->getTokenValue();
    std::cout << pc->getToken()->getTokenTypeString() << Colors::Reset ;

     };

/* ----------------------------------------------------- */
/* PRINTING                                              */
/* ----------------------------------------------------- */
void Interpreter::printCurrentStackFrame(){
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Current Stack Frame:" << std::endl;
    std::cout << "return @ PC " << currentStackFrame->returnPCNum << std::endl;

    std::cout << "___variables____________________" << std::endl;

    for (const auto& item : currentStackFrame->variables) {
        std::cout << item.first << " : ";
        // Handle std::variant int and std::string
        if (std::holds_alternative<int>(item.second)) {
            std::cout << std::get<int>(item.second) << std::endl;
        } else if (std::holds_alternative<std::string>(item.second)) {
            std::cout << std::get<std::string>(item.second) << std::endl;
        }
    }
    std::cout << "--------------------------------" << std::endl;

};
void Interpreter::printResult(){};