#ifndef JUMP_MAP
#define JUMP_MAP

#include <stack>
#include <unordered_map>
#include <iostream>
#include "SymbolTable.h"
#include "AbstractSyntaxTree.h"

// Just a container for the PC location of functions/procedures

// basically we can use the scope of a functions/procedure to get its name
// then we map the name for CALL lookups
// right now it just shows the PC as a number but we probably want to just pass in a
// AST::Node pointer or both the number and the pointer
class JumpMap{

    public:

        struct JumpItem{
            public:
                JumpItem(AbstractSyntaxTree::Node* _pc, int _pcNum, int _scope){
                    pc = _pc;
                    pcNum = _pcNum;
                    scope = _scope;

                };
                AbstractSyntaxTree::Node* pc;
                int pcNum;
                int scope;
        };

        JumpMap(){};
        JumpMap(SymbolTable* _st);
        int scopeCount;

        int getScopeCount() { return scopeCount; };
          // map with a key by name, stores a pointer to the AST location and a numerical program counter
        std::unordered_map<std::string, JumpItem> jumpMap;

        int getPCNum(std::string query);                // returns the numerical PC of a query
        AbstractSyntaxTree::Node* getPC(std::string query); // returns the 
        int getScope(std::string query);

        void add(AbstractSyntaxTree::Node*, int programCounterLocation);
        bool find(std::string query);

        void print();
    private:
        SymbolTable* st;
};
#endif
