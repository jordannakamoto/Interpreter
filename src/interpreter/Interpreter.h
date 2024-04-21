#ifndef INTERPRETER
#define INTERPRETER

#include <string>
#include <iostream>
#include "SymbolTable.h"
#include "AbstractSyntaxTree.h"
#include "JumpMap.h"
#include <vector>
#include <stack>
#include <unordered_map>
#include <variant>
#include "terminalcolors.h"



class Interpreter {

public:

    SymbolTable* st;
    AbstractSyntaxTree& ast;

    Interpreter(SymbolTable* st, AbstractSyntaxTree& ast);

    // StackFrame - stores variable instances for a given function scope
    // returnPC is where we jump to when the call returns
    struct StackFrame {
        std::string name;
        AbstractSyntaxTree::Node* returnPC;
        int returnPCNum;
        Token* returnValue = nullptr;
        std::string returnVarName;
        STEntry* stEntry;

        std::unordered_map<std::string, Token*> variables;

        // init/get/set a variable
        void initVariable(const std::string& name, Token* variableToken){
            variables[name] = variableToken;
        }

        Token* getVariable(const std::string& name) {
            return variables.at(name);
        }
        void setVariable(const std::string& name, std::string value) {
            variables[name]->set_TokenValue(value);
        }

        // get/set the return value
        Token* getReturnValue(){
            return returnValue;
        }
        void setReturnValue(std::string variableName){
            returnValue = getVariable(variableName);
            returnVarName = variableName;
        }
        std::string getReturnValueVarName(){
            return returnVarName;
        }

        // get name of frame (the name of the function)
        std::string getName(){
            return name;
        }

        // ~StackFrame(){
        //     // Because map stores pairs
        //     for (auto& pair : variables) {
        //     // Delete the second item of the pair which is our Token*
        //     delete pair.second;  
        //     }
        // }
    };

    std::string evaluateExpression();
    void evaluateForLoop();
    void evaluateWhileLoop();
    bool evaluateIf();

    void throwDebug(std::string msg);
    void throwDebug(std::string msg, bool flag);

    void pushNewStackFrame(AbstractSyntaxTree::Node* pc, int pcNum, std::string functionName);
    void pushNewGlobalStackFrame();

    void jumpTo(std::string name);
    void jumpToScopeEnd();
    void jumpToElseStatement();


    void preprocess();
    void run();
    Token runCall();
    std::vector<Token> resultValues; // A vector to store return values from evaluating expressions
    // Stored as a token so we can process them like the rest of the expression elements

    void processAssignment();
    void processIfStatement();
    void processForLoop();
    void processWhileLoop();
    void processReturnStatement();

    std::string formatPrintF(std::string, std::vector<std::string>);
    void printCurrentStackFrame();
    void printResult();
    
private:
    int pcNum;                    // Numerical Program Counter
    AbstractSyntaxTree::Node* pc; // Program Counter as an AST Node Pointer
    int pc_END;                   // Numerical Last instruction in program
    JumpMap jumpMap;              // Class that holds all the jump locations for Symbol Table entries
    
    // Stack Frame contains the return to address of the function call
    // And any local variables
    StackFrame* currentStackFrame = nullptr;
    std::vector<StackFrame> callStack;
};

#endif
