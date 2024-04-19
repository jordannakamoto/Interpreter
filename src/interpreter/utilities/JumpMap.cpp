#include "JumpMap.h"

JumpMap::JumpMap(SymbolTable* _st){
    st = _st;
    scopeCount = 0;
}

void JumpMap::add(AbstractSyntaxTree::Node* node,int programCounterLocation){
    scopeCount++;
    const std::string name = st->searchSymbolTableByScope(scopeCount)->getIDName();
    // first line of func/proc definition is at DECLARATION + BEGIN BLOCK 
    jumpMap.insert({name,JumpItem(node, programCounterLocation+1, scopeCount)});
}

// getPCNum - get Numerical PC of an item to jump to
int JumpMap::getPCNum(std::string query){
    return jumpMap.at(query).pcNum;
}
// getPC - get AST Node of an item to jump to
AbstractSyntaxTree::Node* JumpMap::getPC(std::string query){
    return jumpMap.at(query).pc;
}
// getMainPC - get Numerical PC of an item to jump to
int JumpMap::getScope(std::string query){
    return jumpMap.at(query).scope;
}

bool JumpMap::find(std::string query){
    return jumpMap.find(query) != jumpMap.end();
}


void JumpMap::print() {
    std::cout << "Program Counter Location of f/p First Instruction:\n" << std::endl;
    for (const auto& item : jumpMap) {
        std::cout << item.second.pc << " : " << item.first << std::endl;
    }
}