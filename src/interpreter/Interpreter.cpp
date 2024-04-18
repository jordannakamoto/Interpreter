#include "Interpreter.h"

// Constructor
Interpreter::Interpreter(SymbolTable* _st, AbstractSyntaxTree& _ast): st(_st), ast(_ast){
    pc = 0;
    jumpMap = JumpMap(st); // give JumpMap a pointer to the Symbol Table
    
    // Push Global Scope onto Call Stack
    currStackFrame.returnPC = -1; // global scope returns to PC -1 for now...
    callStack.push(currStackFrame);
}

void Interpreter::preProcess(){
// 1. Create Jump Map
// 2. Gather Global Variables Declarations

// The handle for a function or a procedure declaration corresponds
// directly to its scope as recorded in the Symbol Table
// Thus we can store/reference the PC location of the function/procedure by its scope
// Reminder: global variable DECL are always in scope 0, functions/procedures begin at 1

    int temp_pc = 1;    // temporary program counter
    
    throwDebug("hello");
    AbstractSyntaxTree::Node* curr = ast.head;
    Token_Type tt;
    while(curr != nullptr){
        tt = curr->getToken()->getTokenType();
        if(tt == AST_FUNCTION_DECLARATION || tt == AST_PROCEDURE_DECLARATION){
            jumpMap.add(temp_pc);
        }
        // Traverse...
        if(curr->getNextSibling() == nullptr){
            // PC increases with every child of the AST
            curr = curr->getNextChild();
            temp_pc++;
        }
        else{
            // PC also increases for every instruction in an Evaluation or Assignment etc.
            // TODO: we can implement this later...
            curr = curr->getNextSibling();
        }
    }
    pc_END = temp_pc - 1;
    pc_MAIN = jumpMap.getMainPC();

    // DEBUG
    jumpMap.print();
    std::cout << "pc_END: " << pc_END << std::endl;
    std::cout << "pc_MAIN: " << pc_MAIN << std::endl;

    // st->findGlobalVariables();
};

void Interpreter::run(){
    preProcess();

    // simulate registers for expression eval? idk
    std::string a1, a2;

    AbstractSyntaxTree::Node* curr = ast.head;

    Token_Type tt;
    while(curr != nullptr){
        tt = curr->getToken()->getTokenType();
        if(tt == AST_FUNCTION_DECLARATION || tt == AST_PROCEDURE_DECLARATION){
            
        }
        if(curr->getNextSibling() == nullptr){
            // PC increases with every child of the AST
            curr = curr->getNextChild();
            // temp_pc++;
        }
        else{
            // PC also increases for every instruction in an Evaluation or Assignment etc.
            // TODO: we can implement this later...
            curr = curr->getNextSibling();
        }
    }

    // callStack.push();

    // look for procedure main
    while (!callStack.empty()){
        processInstruction();
    }
};

void Interpreter::jumpPC(int pcLoc){
    pc = pcLoc;
}

void Interpreter::processInstruction(){
    // std::string instruction = "";

    // throwDebug("Instruction Ran:");
    pc++; // Increment the program counter.
};

// Throw a debug message to print
void Interpreter::throwDebug(std::string msg){std::cout << msg << std::endl;};

void Interpreter::printResult(){};