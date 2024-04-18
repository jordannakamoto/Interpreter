#ifndef INTERPRETER
#define INTERPRETER

#include <string>
#include <iostream>
#include <stack>
#include "SymbolTable.h"
#include "AbstractSyntaxTree.h"
#include "JumpMap.h"
#include <vector>
#include <unordered_map>
#include <variant>



class Interpreter {

// Our programming language stores data as either
// int or char but I'm just using a string here...
// std::variant allows us to store either type
using VariableType = std::variant<int, std::string>;

public:

    SymbolTable* st;
    AbstractSyntaxTree& ast;

    Interpreter(SymbolTable* st, AbstractSyntaxTree& ast);

    // StackFrame
    // returnPC is where we jump to when the call returns
    // Storage of Variable Instances
    struct StackFrame {
        int returnPC;
        std::unordered_map<std::string, VariableType> variables;

        void defineVariable(const std::string& name, const VariableType& value) {
            variables[name] = value;
        }

        VariableType getVariable(const std::string& name) {
            return variables.at(name);
        }
        // setVariable
    };
    // so actually the callStack can't really be a std::stack since we have to go down
    // and look for variables in other scopes.
    // I think it's called a stack because frames get pushed and popped in order but
    // we're not in assembly so can't access lower frames by incrementing

    void throwDebug(std::string msg);

    void jumpPC(int pcLoc);

    void preProcess();
    void run();
    void processInstruction();

    std::string formatPrintF(std::string, std::vector<std::string>);
    void printCurrStackFrame();
    void printResult();
private:
    int pc;      // Program Counter
    int pc_END;  // Last instruction in program
    // Class that holds all the jump locations for Symbol Table entries
    JumpMap jumpMap;
    // Call Stack just holds the return PC to go to when a call returns
    StackFrame currStackFrame;
    std::stack<StackFrame> callStack;

    // Symbol Lists
    // variable stack
    // pointer to integer or char...

};

#endif
