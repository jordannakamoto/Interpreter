// TokenList.h
// Created by Nick Cabrales on 2/22/24.
// Edited by Jordan Nakamoto

#ifndef ASSIGNMENT2TOKENIZER_TOKENLIST_HPP
#define ASSIGNMENT2TOKENIZER_TOKENLIST_HPP

#include <string>
#include "Token.h"

// Class TokenList

// All nodes are spawned pointing to null sibling ->
class TokenList {
public:
    // :: Node :: 
    // Struct for single Node of Token Linked List
    struct Node {
        Token* token;         // Token pointer
        Node* next = nullptr; // next sibling

        // Getters
        Token* getToken() { return token; }
        Node* getNext() { return next; }

        // Setters
        void setToken(Token* newToken) { token = newToken; }
        void setNext(Node* newSibling) { next = newSibling; }
        ~Node() { delete token; };
    };

    // Constructor
    TokenList(){};
    TokenList(Token* token);

    // Attributes
    Node* head = nullptr; // Head of Token Linked List
    Node* tail = nullptr; // Tail of Token Linked List
    int list_size = 0;
    
    // Functions
    
    // we only need to ever add siblings at tail
    void addNode(Token* token); // void for now

    // Getters
    Node* getHead() { return head; };
    Node* getTail() { return tail; };
    int getListSize() { return list_size; };

    // Setters
    void setHead(Node* node){head = node;};
    void setTail(Node* node){tail = node;};

    // Print Functions
    void printList();

    // ---- Notes ----
    // ---------------
    ~TokenList();

};

#endif //ASSIGNMENT2TOKENIZER_TOKENLIST_HPP

