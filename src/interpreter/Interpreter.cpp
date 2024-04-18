#include "Interpreter.h"

// Constructor
Interpreter::Interpreter(SymbolTable* _st, AbstractSyntaxTree& _ast): st(_st), ast(_ast){
    pc = 0;
    jumpMap = JumpMap(st); // give JumpMap a pointer to the Symbol Table
    
    // Push Global Scope onto Call Stack
    currStackFrame.returnPC = -1; // global scope returns to PC -1 for now...
}

// So first of all I don't think the program counter matters at all since we aren't compiling to machine code
// It's just the notion of pointing to somewhere in the AST to jump to

void Interpreter::preProcess(){
// 1. Create Jump Map

// We can look up what func or procedure we're in when we encounter it by looking up its symbol table entry by scope
// The runtime is probably better if we just attach it when we create the AST but it's fine

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
            // PC theoretically also increases for every instruction in an Evaluation or Assignment etc.
            // but this doesn't actually matter for us unless we want to do a deeper simulation of low level stuff
            curr = curr->getNextSibling();
        }
    }
    pc_END = temp_pc - 1;

    // DEBUG
    jumpMap.print();
    std::cout << "pc_END: " << pc_END << std::endl;

// 2. Gather Global Variables Declarations

    // get all the st entries that are variables in scope 0
    // TODO: break this out into a function
    // so we can use it for the other frames and also have it grab from the params list
    std::vector<STEntry*> results = st->getVariablesByScope(0);
    for(STEntry* entry : results){
        // If we find an integer initialize it at 0 and "" for strings
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

    AbstractSyntaxTree::Node* curr = ast.head;

    pc = jumpMap.getPC("main");
    // here we would want to jump the ast node pointer to the correct position
    // but I haven't added the node pointers to the jump map yet all there is is just a PC number
    // and I realized the numbering doesn't even really matter since the actual program counter
    // is handled by the compiled cpp

    // for the sake of illustration i'll just move the head to the pc at main
    for(int i = 1; i < pc; i++){
        while(curr->getNextSibling()!=nullptr){
            curr = curr->getNextSibling();
        }
        curr = curr->getNextChild();
    }
    // for test1, this should print "DECLARATION" at child 13
    throwDebug("printing the token at pc_MAIN...");
    throwDebug(curr->getToken()->getTokenValue());

    // Here's where we can write the rules for executing expressions and stuff
    // Token_Type tt;

    // AST Loop
    // while(curr != nullptr){
    //     scopeblockcounter = 0;
    //     just ++ this on begin blocks and -- on end blocks
    //     we know if it's 0 then we've been able to skip to the end of an if statement etc.
    //
    //     tt = curr->getToken()->getTokenType();
    //     if(tt == AST_ASSIGNMENT){

    //}
    //     if(tt == AST_CALL){

    //     }
    //     if(tt == AST_IF){

    //}
    //     etc...?
    //     if it's a while or a for loop,
    //     just bookmark the current Node pointer I guess...and move curr back to it lol
    //     
    //     Traverse...
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