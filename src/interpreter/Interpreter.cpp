#include "Interpreter.h"

// !IMPORTANT DISTINCTION
// Token class is used for storage of data in memory
// AS WELL as moving around the AST
// this is because we use it to infer type data
// for variable storage this is the distinction of INTEGER/CHARACTER
// TODO: check that symbol table builds the postfix with CHARACTERS for CHARACTERS...
// otherwise we have to infer from STRINGS and adjust pushNewStackFrame line 283 accordingly
// and for AST traversal this can be a number of things... such as rules for END_BLOCK AST_IF etc.

// Constructor
// Note about Program Counter:
// pc and pcNum represent the marker of where we are in the program's execution

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

    // jumpMap.add: creates an item w/ PC marker, function name retrieved from symbol table

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
// Walk the tree for the within the {  } block of the current function
// returns: the return value of the function
Token Interpreter::runCall()
{   
    // Token provides both our data storage container
    // as well as our traversal mechanism

    // stores what the function will be returning
    Token returnValue;

    // stores the current type of the node being evaluated
    Token_Type tokenType;

    std::stack<char> scopeBlockStack;
    scopeBlockStack.push('{'); // start after the BEGIN_BLOCK
    while (!scopeBlockStack.empty())
    {
        // For tracking parity of {} in IF/ELSE groups
        tokenType = pc->getToken()->getTokenType();
        std::cout << pc->getToken()->getTokenValue() << std::endl;
        switch (tokenType)
        {
        case AST_BEGIN_BLOCK:
            scopeBlockStack.push('{');
            std::cout << "\t+ pushed on { scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
            break;
        case AST_END_BLOCK:
            scopeBlockStack.pop();
            // if we're in global space, we're not looking for an end block
            // global termination handled below in Traverse line 149 with break statement at AST end
            // the print statement above on line 88 will still execute tho to indicate where pc is
            if(currentStackFrame != globalStackFrame){
                std::cout << "\t- popped on } scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
                }
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
            // Similar to for loops
            processWhileLoop();
            break;
        case AST_RETURN:
            processReturnStatement();
            break;
        // case AST_RETURN.... lookup variable from currentStackFrame and return it
        default:
            break;
        };
        
        // ... Traverse
        if (pc->getNextSibling() == nullptr)
        {
            if (pc->getNextChild() != nullptr)
            {
                pc = pc->getNextChild();
                pcNum++; // pcNum at least increases with every child of the AST
            }
            else{ // Terminate call if we're at end of AST ~ end of program
                break;
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
                  << Colors::Magenta << "Returning from call... back to PC: " << Colors::Reset
                  << currentStackFrame->returnPCNum << endl;
    }
    else if (currentStackFrame->returnPCNum == 0)
    {
        std::cout << "-------------------\n"
                  << Colors::Magenta << "Returning from main: " << Colors::Reset
                  << "\n===================" << std::endl;
    }
    if (currentStackFrame->returnPC != nullptr)
    {
        pc = currentStackFrame->returnPC; // move PC to the return destination if there is one
    }
    // SET THE RETURN VALUE BEFORE WE ALTER THE CALL STACK
    // dereference the pointer into an object with *
    // because the stack frame is clearing its memory on pop
    if(currentStackFrame->getReturnValue() != nullptr){
        returnValue = *currentStackFrame->getReturnValue();
        cout << "return: " << currentStackFrame->getReturnValueVarName()
        << " with value: " << currentStackFrame->getReturnValue()->getTokenValue() << endl;
        cout << "===================" << endl;
    }
    // Update the call stack
    callStack.pop_back();                   // pop the current call off the stack since its done
    currentStackFrame = &callStack.back();  // update the currentStackFrame pointer
    // throwDebug("RETURNING FROM CALL");
    return returnValue;
};

// ** Helpers before Eval ** //
// the main run switch-case goes here first before eval
// in case we need any special handling or perhaps don't want to do the implementation in the eval file

void Interpreter::processAssignment(){
    STEntry* tempST = new STEntry();

    pc = pc->getNextSibling();
    cout << Colors::Yellow << pc->getToken()->getTokenValue() << " --- " << pc->getToken()->getTokenType() << Colors::Reset << endl;
    cout << Colors::Magenta << jumpMap.getScopeCount() << Colors::Reset << endl;
    tempST = st->lookupSymbol(pc->getToken()->getTokenValue(), jumpMap.getScopeCount());

    cout << Colors::Cyan << "ST ID_NAME BEFORE: " << tempST->getIDName() << Colors::Reset << endl;
    cout << Colors::Cyan << "ST SCOPE BEFORE: " << tempST->getScope() << Colors::Reset << endl;
    cout << Colors::Cyan << "ST VALUE BEFORE: " << tempST->getValue()->getTokenValue() << Colors::Reset << endl;

    std::string result_msg = evaluateExpression();

    Token* tempToken = new Token(result_msg, NONE, -1);

    if(result_msg != "Returned from Stack"){
        st->lookupSymbol(tempST->getIDName(), jumpMap.getScopeCount())->setValue(tempToken);
        tempST = st->lookupSymbol(tempST->getIDName(), jumpMap.getScopeCount());
    }

    cout << Colors::Yellow << "ST ID_NAME AFTER: " << tempST->getIDName() << Colors::Reset << endl;
    cout << Colors::Cyan << "ST SCOPE AFTER: " << tempST->getScope() << Colors::Reset << endl;
    cout << Colors::Blue << "ST VALUE AFTER: " << tempST->getValue()->getTokenValue() << Colors::Reset << endl;

    // expect some variable to be set by the evaluation
    cout << "\t\t" << Colors::Green << result_msg << Colors::Reset << std::endl;
}

void Interpreter::processIfStatement(){
    bool result;
    pc = pc->getNextSibling();
    // result = evaluateIf();
    // if result == false
    // jumpToElseStatement()
}

void Interpreter::processForLoop(){
    pc = pc->getNextSibling();
    evaluateForLoop();
}

void Interpreter::processWhileLoop(){
    pc = pc->getNextSibling();
    evaluateWhileLoop();
}
void Interpreter::processReturnStatement(){
    pc = pc->getNextSibling();
    // get the name of the variable we're returning
    currentStackFrame->setReturnValue(pc->getToken()->getTokenValue());
}
/* ----------------------------------------------------- */
/* METHODS                                               */
/* ----------------------------------------------------- */

/* -- Navigation -- */

// JumpTo
// Moves the PC pointer to a function in the JumpMap
// args: name - function name to jump to
void Interpreter::jumpTo(std::string name){
    std::cout << Colors::Green << "jumping to..." << name << "*" << Colors::Reset << std::endl;
    pc = jumpMap.getPC(name);
    pcNum = jumpMap.getPCNum(name);
}

// JumpToElseStatement
// Bumps the PC up to the next ELSE statement
// For skipping IF statements
void Interpreter::jumpToElseStatement(){
    Token_Type tt = pc->getToken()->getTokenType();
    while(tt != AST_ELSE){
        pc = pc->getNextChild();
    }
}

// JumpToSkipScope
// Looks for next BEGIN BLOCK
// Bumps the PC up until the matching END BLOCK
// for skipping past ELSE Statements
void Interpreter::jumpToScopeEnd(){
    int bracketCounter = 0;
    bool seenBeginBlock = false;
    while(true){
        if(pc->getToken()->getTokenType() == AST_BEGIN_BLOCK){
            bracketCounter++;
            seenBeginBlock = true;
        }
        else if(pc->getToken()->getTokenType() == AST_END_BLOCK){
            bracketCounter--;
        }
        if(seenBeginBlock && bracketCounter == 0){
            break;
        }
        pc = pc->getNextChild();
    }
}

/* -- Stack Frame -- */
// callStack is implemented as a std::deque
// because std::vectors destroy pointers on resize...

// PushNewStackFrame
void Interpreter::pushNewStackFrame(AbstractSyntaxTree::Node*pc, int pcNum,std::string name){
    StackFrame new_frame(*this);
    new_frame.name = name;
    new_frame.returnPC = pc;
    new_frame.returnPCNum = pcNum;

    int scope = jumpMap.getScope(name);
    new_frame.stEntry = st->searchSymbolTableByScope(scope); // the pointer to the function entry isn't used anywhere yet
    // but the list of variables is...
    std::vector<STEntry*> function_variables = st->getVariablesByScope(scope);
    for(STEntry* entry : function_variables){
        if(entry->getD_Type() == d_int){
            new_frame.initVariable(entry->getIDName(), new Token("0",INTEGER, -1));
        }
        else if(entry->getD_Type() == d_char){
            new_frame.initVariable(entry->getIDName(), new Token("",CHARACTER, -1));
        }
    }
    callStack.push_back(new_frame);
    currentStackFrame = &callStack.back();
    printCurrentStackFrame();
}

// PushNewStackFrame without symbol table handling
// simpler version for the global frame
void Interpreter::pushNewGlobalStackFrame(){
    StackFrame new_global_frame(*this);
    new_global_frame.name = "global";
    new_global_frame.returnPC = nullptr;
    new_global_frame.returnPCNum = -1;   // returns to null AST Node and -1 for now to indicate no return
    
    std::vector<STEntry*> results = st->getVariablesByScope(0);
    for(STEntry* entry : results){
        // Initialize
        // Integers:0 , Strings: ""
        if(entry->getD_Type() == d_int){
            new_global_frame.initVariable(entry->getIDName(), new Token("0",INTEGER, -1));
        }
        else if(entry->getD_Type() == d_char){
            new_global_frame.initVariable(entry->getIDName(), new Token("",CHARACTER, -1));
        }
    }

    callStack.push_back(new_global_frame);
    currentStackFrame = &callStack.back(); // update the variable storing the current frame
    globalStackFrame = &callStack.back();
    printCurrentStackFrame();

}

// Throw a debug message to print in Red
void Interpreter::throwDebug(std::string msg){
    std::cout << Colors::Red << msg << std::endl << Colors::Reset ;
};

// Throw a debug message to print in Red, the current token is printed in red
void Interpreter::throwDebug(std::string msg, bool flag){
    std::cout << Colors::Red << msg << std::endl;
    if(flag){
        std::cout << pc->getToken()->getTokenValue() << " - ";
        std::cout << pc->getToken()->getTokenTypeString() << Colors::Reset << std::endl ;
    }
};

/* ----------------------------------------------------- */
/* PRINTING                                              */
/* ----------------------------------------------------- */
void Interpreter::printCurrentStackFrame(){
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Current Stack Frame:" << std::endl;
    std::cout << '"' << currentStackFrame->getName() << '"' << std::endl;
    std::cout << "return @ PC " << currentStackFrame->returnPCNum << std::endl;

    if(currentStackFrame->variables.size() >0){
        std::cout << "___variables____________________" << std::endl;

        for (const auto& item : currentStackFrame->variables) {
            std::cout << item.first << " : ";
            // Handle std::variant int and std::string
            if (item.second->getTokenType() == INTEGER) {
                std::cout << item.second->getTokenValue() << std::endl;
            } else if (item.second->getTokenType() == CHARACTER) {
                std::cout << item.second->getTokenValue() << std::endl;
            }
        }
    }
    else{
        std::cout << "~no variables" << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
};
void Interpreter::printResult(){};