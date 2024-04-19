#include "Interpreter.h"

// Constructor
// Note about Program Counter:
// pc is the pointer to the AST where the program is currently executing
// pcNum is a numerical representation that could be extended into
// more low level simulation like instruction pointer in a CPU... but for now it's kinda just a lineNumber label for print information
// either way both values are updated throughout the interpretation,
// where pc, or the marker at AST Nodes is the important one for control flow.
Interpreter::Interpreter(SymbolTable* _st, AbstractSyntaxTree& _ast): st(_st), ast(_ast){
    pc = nullptr;
    jumpMap = JumpMap(st);          // link JumpMap to SymbolTable so it can retrieve function names
    pushNewStackFrame(nullptr, -1); // setup global stack frame
}
// Note about Stack Frame:
// The StackFrame holds a function/procedure and instantiation of its variables
// as well as the PC marker it will return to and its return value.
// The variables are stored in a map which you can get by their name
// To be implemented is a simple search function where frames look
// through their ancestor's variables if they can't find the right variable in theirs.
// Which is how we will assign to global variables from within functions.

// ---------------------------------------------------------------- //
// PREPROCESS
// Scan for functions/procedures
// Register the PC of found funcs/procs
// Init global Variables and global Stack Frame
// ---------------------------------------------------------------- //
void Interpreter::preprocess(){
    int temp_pc = 1;    // temporary program counter to number the Map Entries
    
    AbstractSyntaxTree::Node* curr = ast.head;
    Token_Type tt;
    while(curr != nullptr){
        tt = curr->getToken()->getTokenType();
        if(tt == AST_FUNCTION_DECLARATION || tt == AST_PROCEDURE_DECLARATION){
            // Note about JumpMap
            // JumpMap is a very minimal class that just holds
            // a mapping between function/procedure names and their PC marker locations
            // as well as the scope it was identified in so we can do symbol table lookups
            // when the function is called onto the callStack
            jumpMap.add(curr->getNextChild()->getNextChild(),temp_pc); // > puts the PC marker after the BEGIN BLOCK
        }
        // Traverse...
        if(curr->getNextSibling() == nullptr){
            curr = curr->getNextChild();
            temp_pc++; // Numerical PC for labeling the jumpMap increases with every child of the AST
        }
        else{
            // Numerical PC theoretically also increases for every instruction in an Evaluation or Assignment etc.
            // but this doesn't actually matter for us unless we want to do a deeper simulation of low level stuff
            curr = curr->getNextSibling();
        }
    }
    pc_END = temp_pc - 1; // Store the end of program# for debug or something

// 2. Gather Global Variables Declarations

    // get all the st entries that are variables in scope 0
    std::vector<STEntry*> results = st->getVariablesByScope(0);
    for(STEntry* entry : results){
        // Initialize integers at 0 and strings at ""
        // ... since there are no global assignments in the language we don't need to look for them
        if(entry->getD_Type() == d_int){
            currentStackFrame->setVariable(entry->getIDName(), 0);
        }
        else if(entry->getD_Type() == d_char){
            currentStackFrame->setVariable(entry->getIDName(), "");
        }
    }
    // DEBUG
    //jumpMap.print(); std::cout << "pc_END: " << pc_END << std::endl;
    //printCurrentStackFrame();

};

void Interpreter::run(){
    // init AST marker, pc and jump to main before starting run()
    // (main was identified earlier in preprocess, jumpTo references the jumpMap)
    preprocess();
    pc = ast.head;
    jumpTo("main");
    while(!callStack.empty()){
        runCall();
    }
}
// ---------------------------------------------------------------- //
// RUN CALL
// ---------------------------------------------------------------- //
Interpreter::IntOrString Interpreter::runCall(){

    Token_Type tokenType;
    IntOrString returnValue;

    std::stack<char> scopeBlockStack;
    scopeBlockStack.push('{'); // start after the BEGIN_BLOCK
    while(!scopeBlockStack.empty()){
        // For tracking parity of {} in IF/ELSE groups
        tokenType = pc->getToken()->getTokenType();
        throwDebug(pc->getToken()->getTokenValue());
        switch (tokenType){
            case AST_BEGIN_BLOCK:
                scopeBlockStack.push('{');
                std::cout << "\t+ pushed on { scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
                break;
            case AST_END_BLOCK:
                scopeBlockStack.pop();
                std::cout << "\t- popped on } scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
                break;
            case AST_ASSIGNMENT:
                throwDebug("\t> TODO: parse and evaluate an assignment");
                processAssignment();
                break;
            case AST_CALL:
                // callStack.push
                throwDebug("\t> TODO: parse and evaluate a call");
                break;
            case AST_IF: 
                throwDebug("\t> TODO: parse and evaluate an if condition");
                processIfStatement(); // Probably need to pass scopeBlockStack
                // Note: Handling the complexity of the IF/ELSE statement block handling is a significant feature
                // In a program that spends lots of cycles in the same if/else statements
                // or needs to run for a long time or perhaps has large if blocks
                // adding jump markers during preprocessing would probably be wise
                break;
            case AST_FOR: 
                throwDebug("\t> TODO: parse and evaluate a for loop");
                // Note: Furthermore the complexity of for loops recursing back into Ifs is unclear to me presently
                // We could either place them on the call stack or create some struct for them
                // to store their state. There aren't any nested for loops in the tests but we would at least need
                // a vector of whatever struct or data fields they require.
                processForLoop();
                break;
            case AST_WHILE:
                throwDebug("\t> TODO: parse and evaluate a while loop");
                // Similar to while loops
                processWhileLoop();
                break;
            default:
                break;
        };
        // Once again, PC is the marker pointer to the ast
        // like we've called astHead before but if Prof Bruce reads our code
        // he might get some delight
        if(pc->getNextSibling() == nullptr){
            // pcNum at least increases with every child of the AST
            if(pc->getNextChild() != nullptr){
                pc = pc->getNextChild();
                pcNum ++;
            }
        }
        else{
            pc = pc->getNextSibling();
        }
    };
    // * RETURN Routine * //
    // Handle returning from a normal call or a special exit from main
    if(currentStackFrame->returnPCNum != -1){
        std::cout << "-------------------\n" << Colors::Magenta << "Returning from call... back to PC: " << Colors::Reset << currentStackFrame->returnPCNum << "\n===================" << std::endl;
    }
    else{
        std::cout << "Returning from main, exiting program" << std::endl;
    }
    
    pc = currentStackFrame->returnPC; // move PC to the return destination
    callStack.pop_back();             // pop the call off the stack when its done
    currentStackFrame = &callStack.back();
    return "result";    // TODO: return something based on a variable specified in the return statement
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
void Interpreter::pushNewStackFrame(AbstractSyntaxTree::Node*pc, int pcNum){
    StackFrame temp;
    temp.returnPC = pc;
    temp.returnPCNum = pcNum;
    callStack.push_back(temp);
    currentStackFrame = &callStack.back();
}

// Throw a debug message to print
void Interpreter::throwDebug(std::string msg){std::cout << Colors::Reset << msg << Colors::Reset << std::endl;};

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