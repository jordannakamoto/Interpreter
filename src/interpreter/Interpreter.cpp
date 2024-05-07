#include "Interpreter.h"
// Constructor
// Note about Program Counter:
// pc and pcNum represent the marker of where we are in the program's execution

// pc    - Program Counter is the pointer to an AST Node
// pcNum - is a numbered count
Interpreter::Interpreter(SymbolTable* _st, AbstractSyntaxTree& _ast): st(_st), ast(_ast), pc(nullptr), pcNum(1){
    // Toggle Debug Printing Information
    // TOGGLE_BUF = true;
    if (TOGGLE_BUF) {
        tStream.rdbuf(std::cout.rdbuf());
    }
    else {
        tStream.rdbuf(nullptr);
    }
}

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
    printCurrentStackFrame();
    while(!callStack.empty()){
        runCall();
    }
    tStream << "program run complete" << std::endl;
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
        tStream << "WORKING ON " << pc->getToken()->getTokenValue() << std::endl;
        // For tracking parity of {} in IF/ELSE groups
        tokenType = pc->getToken()->getTokenType();
        tStream << pc->getToken()->getTokenValue() << std::endl;
        switch (tokenType)
        {
        case AST_BEGIN_BLOCK:
            scopeBlockStack.push('{');
            tStream << "\t+ pushed on { scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
            break;
        case AST_END_BLOCK:
            scopeBlockStack.pop();
            // if we're in global space, we're not looking for an end block
            // global termination handled below in Traverse line 149 with break statement at AST end
            // the print statement above on line 88 will still execute tho to indicate where pc is
            if(currentStackFrame != globalStackFrame){
                tStream << "\t- popped on } scopeBlockStack size: " << scopeBlockStack.size() << std::endl;
                }
            break;
        case AST_ASSIGNMENT:
            processAssignment();
            break;
        case AST_CALL:
            processCallStatement();
            break;
        case AST_IF:
            processIfStatement(); 
            break;
        case AST_FOR:
            processForLoop();
            break;
        case AST_WHILE:
            processWhileLoop();
            break;
        case AST_RETURN:
            processReturnStatement();
            break;
        case AST_PRINTF:
            processPrintStatement();
            break;
        case AST_ELSE:
            if (!processElse) {
                jumpToScopeEnd();
            }
            break;
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
        tStream << "-------------------\n"
                  << Colors::Magenta << "Returning from call... back to PC: " << Colors::Reset
                  << currentStackFrame->returnPCNum << endl;
    }
    else if (currentStackFrame->returnPCNum == 0)
    {
        tStream << "-------------------\n"
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
        tStream << Colors::Green << "return: " << Colors::Reset << currentStackFrame->getReturnValueVarName()
        << Colors::Green << " with value: " << Colors::Reset << currentStackFrame->getReturnValue()->getTokenValue() << endl;
        tStream << Colors::Reset << "===================" << endl;
    }
    // Update the call stack
    callStack.pop_back();                   // pop the current call off the stack since its done
    if(callStack.size() > 0){
      currentStackFrame = &callStack.back();  // update the currentStackFrame pointer
    }
    return returnValue;
};

// ** Helpers before Eval ** //
// the main run switch-case goes here first before eval, in case we need any special handling or perhaps don't want to do the implementation in the eval file

void Interpreter::processAssignment(){
    pc = pc->getNextSibling();
    std::string result_msg = evaluateExpression();
}

void Interpreter::processForAssignment(){
    auto temp = pc;
    auto var = currentStackFrame->getVariable(temp->getToken()->getTokenValue());
    tStream << "DEBUGGING: FOR ASSIGNMENT: " << var->getTokenValue() << std::endl;
    std::string result_msg = evaluateExpression();
    var = currentStackFrame->getVariable(temp->getToken()->getTokenValue());
    tStream << "DEBUGGING: FOR ASSIGNMENT: " << var->getTokenValue() << std::endl;
}

void Interpreter::processIfStatement(){
    bool result;

    result = evaluateBoolCondition();
    tStream << "\t THE RESULT OF CONDITION WAS: " << result << std::endl; //  DEBUG
    if (!result) {
        pc = pc->getNextChild();
        jumpToScopeEnd();
        processElse = true;
    }
    else {
        processElse = false;
    }
}

void Interpreter::processForLoop(){
    pc = pc->getNextSibling();
    evaluateForLoop();
}

void Interpreter::processWhileLoop(){
    evaluateWhileLoop();
}
void Interpreter::processReturnStatement(){
    pc = pc->getNextSibling();
    // get the name of the variable we're returning
    currentStackFrame->setReturnValue(pc->getToken()->getTokenValue());
}
void Interpreter::processPrintStatement(){
    // Grab arguments
    tStream << Colors::Cyan << "printing... " << std::endl;
    pc = pc->getNextSibling(); // get first argument...
    std::vector<std::string> arguments;

    // gather arguments within (param list) 
    while(true){
        Token* arg = pc->getToken();
        if(arg->getTokenType() == IDENTIFIER){
            Token* variableToken;
            std::string variableValue;
            variableToken = currentStackFrame->getVariable(arg->getTokenValue());
            if(variableToken != nullptr){
                variableValue = variableToken->getTokenValue();
            }
            else{
                variableValue = currentStackFrame->getVarArrayAsString(arg->getTokenValue());
            }
            tStream << Colors::Cyan << "p_arg: "  << Colors::Reset << arg->getTokenValue() << " : " << variableValue << std::endl;
            arguments.push_back(variableValue);
        }
        else{
            tStream << Colors::Cyan << "p_string: "  << Colors::Reset << arg->getTokenValue() << std::endl;
            arguments.push_back(arg->getTokenValue());
        }
        if(pc->getNextSibling() == nullptr){
            break;
        }
        pc = pc->getNextSibling();
    };
    // TODO: substitute for string formatting '%d' etc.
    tStream << Colors::Green << "########## PROGRAM OUTPUT ##########" << Colors::Reset << std::endl;

    int argumentIndex = 1;
    string resultStr;

// Get the first argument from a container named 'arguments'
    resultStr = arguments.at(0);

// Loop through each character in 'resultStr'
    for(int i = 0; i < resultStr.size(); i++){
        // If the current character is '%', replace it with the next argument
        if(resultStr.at(i) == '%'){
            // Remove the '%' character
            resultStr.erase(resultStr.begin() + i);
            // Remove the character that follows '%'
            resultStr.erase(resultStr.begin() + i);

            // Insert the argument at 'argumentIndex' into 'resultStr' at the current position
            resultStr.insert(resultStr.begin() + i, arguments.at(argumentIndex).begin(), arguments.at(argumentIndex).end());
            argumentIndex++;
        }
            // If the current character is '\n', replace it with a newline character
        else if(resultStr.at(i) == '\\' && resultStr.at(i+1) == 'n'){
            // Remove the '\' character
            resultStr.erase(resultStr.begin() + i);
            // Remove the 'n' character
            resultStr.erase(resultStr.begin() + i);
            // Insert a newline character '\n' at the current position
            resultStr.insert(resultStr.begin() + i, '\n');
        }
    }

// Update the first argument in the 'arguments' container with the modified 'resultStr'
    arguments.at(0) = resultStr;

    tStream << Colors::Yellow << "====================================" << Colors::Reset << std::endl;
    outstring += resultStr;

    // Print the FINAL RESULT!
    std::cout << resultStr;

    tStream << Colors::Yellow << "====================================" << Colors::Reset << std::endl;
    throwDebug(pc->getNextChild()->getToken()->getTokenValue());

}

void Interpreter::processCallStatement(){
        pc = pc->getNextSibling();
        std::string tokenValue = pc->getToken()->getTokenValue();
        // tokenValue should now hold name of function

        if(jumpMap.find(tokenValue)){
            tStream <<  "\n===========\n" << Colors::Magenta  << "CALL Statement found, Pushing " << tokenValue << " to Call Stack" << Colors::Reset << std::endl;
        
            // Grab arguments before calling
            pc = pc->getNextSibling(); // go to first argument
            throwDebug(pc->getToken()->getTokenValue());

            // IMPORTANT: NOT SUPPORTING ARRAYS FOR SIMPLICITY TO DEBUG AND PASS TEST 3
            std::vector<std::string> arguments;
            while(true){
                Token* param = pc->getToken();
                    if(param->getTokenType() == IDENTIFIER){
                        std::string variableValue = currentStackFrame->getVariable(param->getTokenValue())->getTokenValue();
                        arguments.push_back(variableValue);
                    }
                    else{
                        arguments.push_back(param->getTokenValue());
                    }
                if(pc->getNextSibling() == nullptr){
                    break;
                }
                pc = pc->getNextSibling();
            }
            pushNewStackFrame(pc,pcNum,tokenValue);
            for(int i = 0; i < arguments.size();i++){
                currentStackFrame->setParameter(i, arguments[i]);
            }
            printCurrentStackFrame();
            jumpTo(tokenValue);
            runCall();
        }
}
/* ----------------------------------------------------- */
/* METHODS                                               */
/* ----------------------------------------------------- */

/* -- Navigation -- */

// JumpTo
// Moves the PC pointer to a function in the JumpMap
// args: name - function name to jump to
void Interpreter::jumpTo(std::string name){
    tStream << Colors::Green << "jumping to..." << name << "*" << Colors::Reset << std::endl;
    pc = jumpMap.getPC(name);
    pcNum = jumpMap.getPCNum(name);
}

// JumpToElseStatement
// Not Used
// Bumps the PC up to the next ELSE statement
// For skipping IFELSE statements
void Interpreter::jumpToElseStatement(){
    Token_Type tt = pc->getToken()->getTokenType();
    while(tt != AST_ELSE && (pc->getNextChild() != nullptr)){
        pc = pc->getNextChild();
        Token_Type tt = pc->getToken()->getTokenType();
    }
}

// JumpToScopeEnd
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
        
        // traverse the linked list
        while (pc->getNextSibling() != nullptr) {
            pc = pc->getNextSibling();
        }
        pc = pc->getNextChild();
        if (pc == nullptr) {
            std::cerr << "jump to scope end not working" << std::endl;
            throw(-1);
        }
    }
    tStream << "successful jump" << std::endl;
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
        // instantiate arrays or single variables
        int arraySize = entry->getArraySize();
        if(entry->getD_Type() == d_int){
            if(arraySize > 0){
                new_frame.initArrayVariable(entry->getIDName(), new Token("0",INTEGER,-1),arraySize);
            }
            else{
                new_frame.initVariable(entry->getIDName(), new Token("0",INTEGER, -1));
            }
        }
        else if(entry->getD_Type() == d_char){
            if(arraySize > 0){
                new_frame.initArrayVariable(entry->getIDName(), new Token("",CHARACTER,-1),arraySize);
            }
            else{
                new_frame.initVariable(entry->getIDName(), new Token("",CHARACTER, -1));
            }
        }
    }
    // then get the parameters, in the test cases there are no parameters with arrays :)
    std::vector<STEntry*> function_parameters = st->getParametersByScope(scope);
    for(STEntry* entry : function_parameters){
        if(entry->getD_Type() == d_int){
            new_frame.initVariable(entry->getIDName(), new Token("0",INTEGER, -1));
        }
        else if(entry->getD_Type() == d_char){
            new_frame.initVariable(entry->getIDName(), new Token("",CHARACTER, -1));
        }
        new_frame.initParameter(entry->getIDName()); // Creates an indexed list for parameters... workaround
    }
    callStack.push_back(new_frame);
    currentStackFrame = &callStack.back();
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
    tStream << Colors::Red << msg << std::endl << Colors::Reset ;
};

// Throw a debug message to print in Red, the current token is printed in red
void Interpreter::throwDebug(std::string msg, bool flag){
    tStream << Colors::Red << msg << std::endl;
    if(flag){
        tStream << pc->getToken()->getTokenValue() << " - ";
        tStream << pc->getToken()->getTokenTypeString() << Colors::Reset << std::endl ;
    }
};

/* ----------------------------------------------------- */
/* PRINTING                                              */
/* ----------------------------------------------------- */
void Interpreter::printCurrentStackFrame(){
    tStream << "--------------------------------" << std::endl;
    tStream << "Current Stack Frame:" << std::endl;
    tStream << '"' << currentStackFrame->getName() << '"' << std::endl;
    tStream << "return @ PC " << currentStackFrame->returnPCNum << std::endl;

    if(currentStackFrame->variables.size() >0){
        tStream << "___variables____________________" << std::endl;

        for (const auto& item : currentStackFrame->variables) {
            tStream << item.first << " : ";
            if(item.second->getTokenType() == CHARACTER){
                tStream << "'" << item.second->getTokenValue() << "'";
            }
            else{
                tStream << item.second->getTokenValue();
            }

            for (size_t i = 0; i < currentStackFrame->parameters.size(); ++i) {
                const auto& param = currentStackFrame->parameters[i];
                if(item.first == param){
                    tStream << " (parameter " << i+1 << ")";
                }
            }
            tStream << std::endl;
        }
        // now do array variables
        for (const auto& item : currentStackFrame->array_variables) {
            tStream << item.first << " : (array) size -";
            tStream << item.second.size();
            tStream << std::endl;
        }
    }
    else{
        tStream << "~no variables" << std::endl;
    }
    tStream << "--------------------------------" << std::endl;
};
void Interpreter::printResult(){};
