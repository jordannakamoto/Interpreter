#ifndef INTERPRETER
#define INTERPRETER

#include <string>
#include <iostream>
#include "SymbolTable.h"
#include "AbstractSyntaxTree.h"
#include "JumpMap.h"
#include <vector>
#include <deque>
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
        Interpreter& interpreter;
        // CallStack should probably be its own class... which contains this struct for StackFrames
        // but for now we can just pass in the interpreter to reference global frame when looking for variables

        std::unordered_map<std::string, Token*> variables;
        //array variables
        std::vector<std::string> parameters;

        // Constructor has to pass the interpreter instance so we can access the callStack...
        explicit StackFrame(Interpreter& i) : interpreter(i) {}

        // init/get/set a variable
        void initVariable(const std::string& name, Token* variableToken){
            variables[name] = variableToken;
        }
        void initParameter(const std::string& name){
            parameters.push_back(name);
        }

        /* StackFrame::getVariable */
        // Usage: currentStackFrame->getVariable("x");
        Token* getVariable(const std::string& name) {
            // First check in the current stack frame
            auto it = variables.find(name);
            if (it != variables.end()) {
                return it->second;
            }

            // If not found, check in the global stack frame
            it = interpreter.globalStackFrame->variables.find(name);
            if (it != interpreter.globalStackFrame->variables.end()) {
                return it->second;
            }
            else{
                std::cout << "couldn't get variable, it wasn't found" << std::endl;
            }
            return nullptr;
        }

        /* StackFrame::setVariable */
        // Usage: currentStackFrame->setVariable("x", "1");
        void setVariable(const std::string& name, const std::string& value) {
            auto it = variables.find(name);
            if (it != variables.end()) {
                std::cout << "setting variable " << Colors::Yellow << name << Colors::Reset << " to " << Colors::Yellow << value <<  std::endl;
                it->second->set_TokenValue(value);
                return;
            }
            // If not found, set in the global stack frame
            it = interpreter.globalStackFrame->variables.find(name);
                if (it != interpreter.globalStackFrame->variables.end()) {
                    it->second->set_TokenValue(value);
                }
                else{
                    std::cout << "couldn't set variable, it wasn't found" << std::endl;
                }
        }

        /* StackFrame::setParameter */
        // Usage: currentStackFrame->setParameter("0","hello");
        // refer to parameter by its index, and then set the corresponding variable
        // i.e. foo(n,h) where n is param 1 at index 0, h is param 2 at index 1
        void setParameter(const int index, std::string value){
            std::cout << "setting parameter " << index << " , " << parameters[index] << " to " << value << std::endl;
            setVariable(parameters[index], value);
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

        /* NOTE:
        I believe the cpp 20 compiler will automatically handle memory clean up
        of pointers in certain circumstances because running an explicit destructor
        gives a memory already freed error
        */
        // ~StackFrame(){
        //     // Because map stores pairs
        //     for (auto& pair : variables) {
        //     // Delete the second item of the pair which is our Token*
        //     delete pair.second;  
        //     }
        // }
    };

    void evaluateIdentifier();
    std::string evaluateExpression();
    void evaluateForLoop();
    void evaluateWhileLoop();
    bool evaluateIf();

    void throwDebug(std::string msg);
    void throwDebug(std::string msg, bool flag);

    void pushNewStackFrame(AbstractSyntaxTree::Node* pc, int pcNum, std::string functionName);
    void pushNewGlobalStackFrame();

    Token* getVariable(const std::string& name);
    void setVariable(const std::string&name, const std::string& value);

    void jumpTo(std::string name);
    void jumpToScopeEnd();
    void jumpToElseStatement();


    void preprocess();
    void run();
    Token runCall();
    std::vector<Token> resultValues; // A vector to store return values from evaluating expressions, experimental: not needed

    void processAssignment();
    void processIfStatement();
    void processForLoop();
    void processWhileLoop();
    void processReturnStatement();
    void processPrintStatement();

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
    StackFrame* globalStackFrame = nullptr;
    std::deque<StackFrame> callStack;
};

#endif

