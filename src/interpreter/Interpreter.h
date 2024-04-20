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

// Our programming language stores data as either
// int or char but I'm just using a string here...
// std::variant allows us to store either type
using IntOrString = std::variant<int, std::string>;

public:

    SymbolTable* st;
    AbstractSyntaxTree& ast;

    Interpreter(SymbolTable* st, AbstractSyntaxTree& ast);

    struct StackFrame;
    // StackFrame - stores variable instances for a given function scope
    // returnPC is where we jump to when the call returns
    struct StackFrame {
        AbstractSyntaxTree::Node* returnPC;
        int returnPCNum;
        IntOrString returnValue;
        STEntry* stEntry;

        std::unordered_map<std::string, IntOrString> variables;

        // define/update a variable
        void setVariable(const std::string& name, const IntOrString& value) {
            variables[name] = value;

            // > Demonstration of printing a value from the variant container
            // Using std::visit to basically evaluate what's inside.
            // based on the auto datatype. There are other ways too...
            // std::visit([name](auto&& arg) {
            //     std::cout << "Variable: " << name << ", set to: " << arg << std::endl;
            // }, value);
        }

        IntOrString getVariable(const std::string& name) {
            return variables.at(name);
        }

    };

    IntOrString evaluateExpression();
    void evaluateForLoop();
    void evaluateWhileLoop();
    void evaluateIf();

    void throwDebug(std::string msg);

    void pushNewStackFrame(AbstractSyntaxTree::Node* pc, int pcNum, std::string functionName);
    void pushNewGlobalStackFrame();

    void jumpTo(std::string name);
    void jumpToScopeEnd();
    void jumpToElseStatement();


    void preprocess();
    void run();
    IntOrString runCall();
    std::vector<Token*> resultValues; // A vector to store return values from evaluating expressions
    // Stored as a token so we can process them like the rest of the expression elements

    void processAssignment();
    void processIfStatement();
    void processForLoop();
    void processWhileLoop();

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
