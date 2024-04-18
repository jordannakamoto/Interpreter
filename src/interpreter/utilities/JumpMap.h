#ifndef JUMP_MAP
#define JUMP_MAP

#include <stack>
#include <unordered_map>
#include <iostream>
#include "SymbolTable.h"

// Just a container for the PC location of functions/procedures

// basically we can use the scope of a functions/procedure to get its name
// then we map the name for CALL lookups
class JumpMap{

    public:

        JumpMap(){};
        JumpMap(SymbolTable* _st);
        int scopeCount;
        std::unordered_map<std::string, int> jumpMap;

        int getPC(std::string query); // returns the PC of a query

        void add(int programCounterLocation);

        void print();
    private:
        SymbolTable* st;
};
#endif