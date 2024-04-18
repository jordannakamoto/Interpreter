#ifndef JUMP_MAP
#define JUMP_MAP

#include <stack>
#include <vector>
#include <iostream>
#include "SymbolTable.h"

// Just a container for the PC location of functions/procedures

// - Not actually a map, just a vector
// basically we can use the scope of a functions/procedure as its unique key
// to both find and access the PC for when we need to jump somewhere
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