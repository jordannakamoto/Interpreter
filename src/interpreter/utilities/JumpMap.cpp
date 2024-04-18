#include "JumpMap.h"

JumpMap::JumpMap(SymbolTable* _st){
    st = _st;
    scopeCount = 0;
}

void JumpMap::add(int programCounterLocation){
    scopeCount++;
    const std::string name = st->searchSymbolTableByScope(scopeCount)->getIDName();
    // first line of func/proc definition is at DECLARATION + BEGIN BLOCK 
    jumpMap.insert({name,programCounterLocation+2});
}

// getMainPC - Return PC of an item
int JumpMap::getPC(std::string query){
    return jumpMap.at(query);
}

void JumpMap::print() {
    std::cout << "Program Counter Location of f/p First Instruction:\n" << std::endl;
    for (const auto& item : jumpMap) {
        std::cout << item.second << " : " << item.first << std::endl;
    }
}