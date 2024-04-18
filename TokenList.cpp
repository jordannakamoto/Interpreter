#include "TokenList.h"
#include <iostream>

// Constructor
TokenList::TokenList(Token* token) {
    Node* newNode = new Node();
    newNode->setToken(token);
    // Set initial head and tail of newly constructed TokenList!
    head = newNode;
    tail = newNode;
    list_size = 1;
}

// Add Nodes to List ...
void TokenList::addNode(Token* token) {
    // 1. Make new Node
    // 2. Set new Node's token
    // 3. Connect new Node to List via tail.next
    // 4. Set new tail and increment list_size

    Node* newNode = new Node();
    newNode->setToken(token);
    if (tail) {
        tail->setNext(newNode);
        tail = newNode;
        list_size++;    
    } else { // if uninitialized, spawn at head
        head = newNode;
        tail = newNode;
        list_size = 1;
    }
}

void TokenList::printList(){
    Node* curr = head;
    while(curr != nullptr){
        std::cout << "token type: " << curr->getToken()->getTokenTypeString() << " ";
        std::cout << "\ttoken value: " << curr->getToken()->getTokenValue() << "\n";
        curr = curr->getNext();
    }
    std::cout << "\nList Printed..." << std::endl;
}

TokenList::~TokenList(){
    Node* tempPrev;
    Node* tempNext;
    tempPrev = this->getHead();
    while(tempPrev != nullptr) {
        tempNext = tempPrev->getNext();
        delete tempPrev;
        tempPrev = tempNext;
    }
}