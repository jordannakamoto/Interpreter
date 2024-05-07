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
#include <ostream>
#include "terminalcolors.h"
#include "ToggleBuffer.h"



class Interpreter {

public:
    bool TOGGLE_BUF = false;
    std::ostream& tStream = std::cerr;
    std::string outstring; // for producing an output file
    SymbolTable* st;
    AbstractSyntaxTree& ast;

    Interpreter(SymbolTable* st, AbstractSyntaxTree& ast);

    // StackFrame - stores variable instances for a given function scope
    // returnPC is where we jump to when the call returns
    struct StackFrame {
        bool TOGGLE_BUF = false;
        std::string name;
        AbstractSyntaxTree::Node* returnPC;
        int returnPCNum;
        Token* returnValue = nullptr;
        std::string returnVarName;
        STEntry* stEntry;
        Interpreter& interpreter;
        std::ostream& tStream = std::cerr;
        // CallStack should probably be its own class... which contains this struct for StackFrames
        // but for now we can just pass in the interpreter to reference global frame when looking for variables

        std::unordered_map<std::string, Token*> variables;
        std::unordered_map<std::string, std::vector<Token*>> array_variables;
        std::vector<std::string> parameters;

        // Constructor has to pass the interpreter instance so we can access the callStack...
        explicit StackFrame(Interpreter& i) : interpreter(i) {
            if (TOGGLE_BUF) {
                tStream.rdbuf(nullptr);
            }
        }

        /* --- Variable/Parameter Memory Management --- */
        void initVariable(const std::string& name, Token* variableToken){
            variables[name] = variableToken;
        }
        void initArrayVariable(const std::string& name, Token* variableToken, const int size){
            // assuming we init with a blank token with the desired Token_Type
            // either INTEGER or CHARACTER
            for(int i = 0; i < size;i++){
                array_variables[name].push_back(variableToken->copy()); // populate with that blank token
            }
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
                tStream << "get var not found, try searching array_variables" << std::endl;
            }
            return nullptr;
        }

        /* StackFrame::setVariable */
        // Usage: currentStackFrame->setVariable("x", "1");
        void setVariable(const std::string& name, const std::string& value) {
            auto it = variables.find(name);
            if (it != variables.end()) {
                tStream << "setting variable " << Colors::Yellow << name << Colors::Reset << " to " << Colors::Yellow << value <<  std::endl << Colors::Reset;
                it->second->set_TokenValue(value);
                return;
            }
            // If not found, set in the global stack frame
            it = interpreter.globalStackFrame->variables.find(name);
                if (it != interpreter.globalStackFrame->variables.end()) {
                    it->second->set_TokenValue(value);
                }
                else{
                    tStream << "set var not found, try setArrayVariableFromString()" << std::endl;
                }
        }

        /* StackFrame::setParameter */
        // Usage: currentStackFrame->setParameter("0","hello");
        // refer to parameter by its index, and then set the corresponding variable
        // i.e. foo(n,h) where n is param 1 at index 0, h is param 2 at index 1
        void setParameter(const int index, std::string value){
            tStream << Colors::Black << "passing parameter " << index+1 << "'s" //<< parameters[index] << " to " << value << Colors::Reset << std::endl;
            << " variable..." << Colors::Reset << std::endl;
            setVariable(parameters[index], value);
        }

        /* ... Array Memory Management */
        // varName of an array variable, index for setting, value to be set
        void setVarArrayValue(const std::string&name, const int index, const std::string value){
            array_variables[name][index]->set_TokenValue(value);
        }
        // fill an array_variable of characters from a string
        void setArrayVariableFromString(const std::string&name, const std::string str){
            if(array_variables[name][0]->getTokenType() != CHARACTER){
                __throw_runtime_error("Attempted to fill a non-CHARACTER array with a string");
            }
            std::vector<Token*> target = array_variables[name];
            int n = 0;
            for(int i = 0; i < str.size(); i ++, n++){
                std::string character = str.substr(i,1);
                if(character == "\\"){
                    if (str.substr(i,3) == "\\x0"){ // lookahead for null terminator if we see an escape
                        target[i]->set_TokenValue("\\x0"); // set the null terminator as the token value and stop processing
                        break;
                    }
                }
                target[i]->set_TokenValue(str.substr(i,1)); //index, length of substr
            }
            tStream << "setting" << Colors::Magenta << " array "  << Colors::Reset <<  "variable " << Colors::Yellow << name << Colors::Reset << " to " << Colors::Yellow << "\""  << str.substr(0,n) << "\"" << std::endl;
            // test for test2
            tStream << Colors::Black << "Verifying char array storage of array_variable: " << name << Colors::Reset <<  std::endl;
            int j = 0;  // Ensure 'j' is initialized before using it
            std::string val;
            while ((val = getVarArray(name, j)->getTokenValue()) != "\\x0") {  // Correct parentheses and assignment
                tStream << Colors::Black << val << ",";
                j++;  // Increment 'j' to advance through the array
            }
            tStream << Colors::Reset;
        }
        Token* getVarArray(const std::string&name, const int index){
            return array_variables[name][index];
        }

        std::string getVarArrayAsString(const std::string&name){
            std::string result;
            std::vector<Token*> array  = array_variables[name];
            for(int i = 0; i < array.size();i++){
                if(array[i]->getTokenValue() == "\\x0"){
                    break;
                }
                result += array[i]->getTokenValue();
            }
            return result;
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
    void processForAssignment();
    void processIfStatement();
    void processForLoop();
    void processWhileLoop();
    void processReturnStatement();
    void processPrintStatement();
    void processCallStatement();
    bool evaluateBoolCondition();

    std::string formatPrintF(std::string, std::vector<std::string>);
    void printCurrentStackFrame();
    void printResult();
    
private:

    int pcNum;                    // Numerical Program Counter
    AbstractSyntaxTree::Node* pc; // Program Counter as an AST Node Pointer
    int pc_END;                   // Numerical Last instruction in program
    JumpMap jumpMap;              // Class that holds all the jump locations for Symbol Table entries
    bool processElse = false;
    
    // Stack Frame contains the return to address of the function call
    // And any local variables
    StackFrame* currentStackFrame = nullptr;
    StackFrame* globalStackFrame = nullptr;
    std::deque<StackFrame> callStack;
};

#endif

