#include "Interpreter.h"

// Constructor
Interpreter::Interpreter(SymbolTable* _st, AbstractSyntaxTree& _ast): st(_st), ast(_ast){
    pc = 0;
    jumpMap = JumpMap(st);          // link JumpMap to SymbolTable so it can retrieve function names
    pushNewStackFrame(nullptr, -1); // setup global stack frame with returnPC at -1 and null AST Node pointer
}


// ---------------------------------------------------------------- //
// PREPROCESS
// Scan for functions/procedures
// Register the AST node of found funcs/procs as well as numerical PC.
// Init global Variables and global Stack Frame
// ---------------------------------------------------------------- //
void Interpreter::preprocess(){
    int temp_pc = 1;    // temporary program counter to number the Map Entries
    
    AbstractSyntaxTree::Node* curr = ast.head;
    Token_Type tt;
    while(curr != nullptr){
        tt = curr->getToken()->getTokenType();
        if(tt == AST_FUNCTION_DECLARATION || tt == AST_PROCEDURE_DECLARATION){
            jumpMap.add(curr->getNextChild()->getNextChild(),temp_pc); // start after the BEGIN BLOCK
        }
        // Traverse...
        if(curr->getNextSibling() == nullptr){
            curr = curr->getNextChild();
            temp_pc++; // PC increases with every child of the AST
        }
        else{
            // PC theoretically also increases for every instruction in an Evaluation or Assignment etc.
            // but this doesn't actually matter for us unless we want to do a deeper simulation of low level stuff
            curr = curr->getNextSibling();
        }
    }
    pc_END = temp_pc - 1; // Store the numerical end of program

// 2. Gather Global Variables Declarations

    // get all the st entries that are variables in scope 0
    std::vector<STEntry*> results = st->getVariablesByScope(0);
    for(STEntry* entry : results){
        // If we find an integer initialize it at 0 and "" for strings
        // ... since there are no global assignments in CST we don't need to look for them
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
    // set pc to the start and jump to main before starting run()
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
                processIfStatement();
                break;
            case AST_FOR: 
                throwDebug("\t> TODO: parse and evaluate a for loop");
                processForLoop();
                break;
            case AST_WHILE:
                throwDebug("\t> TODO: parse and evaluate a while loop");
                processWhileLoop();
                break;
            default:
                break;
        };
        if(pc->getNextSibling() == nullptr){
            // PC increases with every child of the AST
            if(pc->getNextChild() != nullptr){
                pc = pc->getNextChild();
            }
        }
        else{
            // PC also increases for every instruction in an Evaluation or Assignment etc.
            // TODO: we can implement this later...
            pc = pc->getNextSibling();
        }
    };
    if(currentStackFrame->returnPCNum != -1){
        std::cout << "-------------------\n" << Colors::Magenta << "Returning from call... back to PC: " << Colors::Reset << currentStackFrame->returnPCNum << "\n===================" << std::endl;
    }
    else{
        std::cout << "Returning from main, exiting program" << std::endl;
    }
    pc = currentStackFrame->returnPC; // set PC to the return destination
    callStack.pop_back(); // pop the call off the stack when its done
    currentStackFrame = &callStack.back();
    return "result";
};

// ** Helpers before Eval ** //
// in case we need any special handling

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


// JumpTo
// Moves the PC pointer to a pointer in the map
// Updates the numerical PC representation as well
void Interpreter::jumpTo(std::string name){
    std::cout << Colors::Green << "jumping to..." << name << "*" << Colors::Reset << std::endl;
    pc = jumpMap.getPC(name);
    pcNum = jumpMap.getPCNum(name);
}

void Interpreter::pushNewStackFrame(AbstractSyntaxTree::Node*pc, int pcNum,std::string name){
    StackFrame temp;
    temp.returnPC = pc;
    temp.returnPCNum = pcNum;
    int function_scope = jumpMap.getScope(name);
    temp.stEntry = st->searchSymbolTableByScope(function_scope);
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

void Interpreter::pushNewStackFrame(AbstractSyntaxTree::Node*pc, int pcNum){
    StackFrame temp;
    temp.returnPC = pc;
    temp.returnPCNum = pcNum;
    callStack.push_back(temp);
    currentStackFrame = &callStack.back();
}

Interpreter::IntOrString Interpreter::getVariable(std::string name){
    
}

void Interpreter::setVariable(std::string name){

}




// Maybe use this for Expression Evaluation
void Interpreter::processInstruction(){
    // std::string instruction = "";

    // throwDebug("Instruction Ran:");
    pc++; // Increment the program counter.
};

// Throw a debug message to print
void Interpreter::throwDebug(std::string msg){std::cout << msg << std::endl;};

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