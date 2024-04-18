#include "JumpMap.h"

// Just a container for the PC location of functions/procedures
JumpMap::JumpMap(SymbolTable* _st){
    st = _st;
    scopeCount = 0;
    // Give global scope a dummy value because func/procedure scopes start at 1
    jumpMap.push_back(JumpItem(0,0,nullptr)); 
}

void JumpMap::add(int programCounterLocation){
    scopeCount++;
    // first line of func/proc definition is at DECLARATION + BEGIN BLOCK 
    JumpItem newItem = JumpItem(scopeCount, programCounterLocation+2, st->searchSymbolTableByScope(scopeCount));
    jumpMap.push_back(newItem);
}

// getMainPC - Return PC of procedure main
int JumpMap::getMainPC(){
    for(int i =1; i < jumpMap.size(); i++){
        if(jumpMap[i].stEntry->getIDName() == "main"){
            // Validating that there is only one procedure main is done previously during parsing.
            return jumpMap[i].programCounterLocation;
        }
    }
    return -1;
}

void JumpMap::print(){
    std::cout << "Program Counter Location of f/p First Instruction:\n" << std::endl;
    for(int i = 1; i < jumpMap.size(); i++){ // skip the global scope at 0
        std::cout << jumpMap[i].programCounterLocation << " : " << jumpMap[i].stEntry->getIDName() << std::endl;
        // debug
        std::cout << "     at scope: " << jumpMap[i].scopeValue << std::endl << std::endl;
    }
}