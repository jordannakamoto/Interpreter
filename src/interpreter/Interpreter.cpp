#include "Interpreter.h"

// Constructor
Interpreter::Interpreter(SymbolTable* _st, AbstractSyntaxTree& _ast): st(_st), ast(_ast){
    pc = 0;
    jumpMap = JumpMap(st); // give JumpMap a pointer to the Symbol Table
    
    // Push Global Scope onto Call Stack
    currStackFrame.returnPC = -1; // global scope returns to PC -1 for now...
}

void Interpreter::preProcess(){
// 1. Create Jump Map

// The handle for a function or a procedure declaration corresponds
// directly to its scope as recorded in the Symbol Table
// Thus we can store/reference the PC location of the function/procedure by its scope
// Reminder: global variable DECL are always in scope 0, functions/procedures begin at 1

    int temp_pc = 1;    // temporary program counter
    
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
            // TODO: we can implement this later... some notes are below about using mod% 3
            curr = curr->getNextSibling();
        }
    }
    pc_END = temp_pc - 1;

    // DEBUG
    jumpMap.print();
    std::cout << "pc_END: " << pc_END << std::endl;

// 2. Gather Global Variables Declarations

    // find Global by scope 0;
    std::vector<STEntry*> results = st->getVariablesByScope(0);
    for(STEntry* entry : results){
        // Initialize global variables with values 0 and ""
        // Since the programming language doesn't support global assignments...
        // Well we might want a declareVariable method for the stack frame
        // instead of just define
        // But this should work and we can verify that
        // a stack frame can access its variables
        if(entry->getD_Type() == d_int){
            currStackFrame.defineVariable(entry->getIDName(), 0);
        }
        else if(entry->getD_Type() == d_char){
            currStackFrame.defineVariable(entry->getIDName(), "");
        }
    }
    printCurrStackFrame();
    callStack.push_back(currStackFrame);
};

void Interpreter::run(){
    preProcess();

    // simulate registers for expression eval? idk
    std::string a1, a2;

    AbstractSyntaxTree::Node* curr = ast.head;

    pc = jumpMap.getPC("main");
    // here we would want to jump the ast node pointer to the correct position
    // but we have no way of doing so at the moment...
    // to complete the assignment we only need the program counter to deal with children
    // we could calculate the PC for siblings by doing 
    // modulo %3 of the postfix count for operand operator operand

    // for the sake of illustration i'll just move the head to the pc at main
    // since right now PC only counts child connections
    for(int i = 1; i < pc; i++){
        while(curr->getNextSibling()!=nullptr){
            curr = curr->getNextSibling();
        }
        curr = curr->getNextChild();
    }
    // for test1, this should print "DECLARATION" at child 13
    throwDebug("printing the token at pc_MAIN...");
    throwDebug(curr->getToken()->getTokenValue());

    // An easy way is to have a vector corresponding to scope of AST::Node pointers
    // that get dropped by the preprocessor...
    // because our scopes once again don't nest beyond the first function
    // the iteration of the program counter is kinda meaningless in cpp anyways but
    // to have it function more like assembly we would need the AST to be a doubly linked list

    // Token_Type tt;

    // AST Loop
    // while(curr != nullptr){
    //     tt = curr->getToken()->getTokenType();
    //     if(tt == AST_FUNCTION_DECLARATION || tt == AST_PROCEDURE_DECLARATION){
            
    //     }
    //     if(curr->getNextSibling() == nullptr){
    //         // PC increases with every child of the AST
    //         curr = curr->getNextChild();
    //         // temp_pc++;
    //     }
    //     else{
    //         // PC also increases for every instruction in an Evaluation or Assignment etc.
    //         // TODO: we can implement this later...
    //         curr = curr->getNextSibling();
    //     }
    // }

    // callStack.push();


    // while (!callStack.empty()){
    //     processInstruction();
    // }
};

void Interpreter::jumpPC(int pcLoc){
    pc = pcLoc;
}

// Maybe use this for Expression Evaluation
void Interpreter::processInstruction(){
    // std::string instruction = "";

    // throwDebug("Instruction Ran:");
    pc++; // Increment the program counter.
};

// Throw a debug message to print
void Interpreter::throwDebug(std::string msg){std::cout << msg << std::endl;};

void Interpreter::printCurrStackFrame(){
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Current Stack Frame:" << std::endl;
    std::cout << "return PC: " << currStackFrame.returnPC << std::endl;

    std::cout << "Variables in current stack frame:" << std::endl;

    for (const auto& item : currStackFrame.variables) {
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