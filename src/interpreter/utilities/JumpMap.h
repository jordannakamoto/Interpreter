#ifndef JUMP_MAP
#define JUMP_MAP

#include <stack>
#include <vector>
#include <iostream>
#include "SymbolTable.h"

// Not actually a map, just a vector
// add an STEntry to the jumpMap

// basically we can use the scope to key functions/procedures in the symbol table
class JumpMap{

    public:
        struct JumpItem{
            int scopeValue;
            int programCounterLocation;
            STEntry* stEntry;

            JumpItem(int scope,int pcLoc, STEntry* entry) {
                scopeValue = scope;
                programCounterLocation = pcLoc;
                stEntry = entry;
            };
        };

        JumpMap(){};
        JumpMap(SymbolTable* _st);
        int scopeCount;
        std::vector<JumpItem> jumpMap;

        int getMainPC(); // returns the PC of the main procedure

        void add(int programCounterLocation);

        void print();
    private:
        SymbolTable* st;
};
#endif