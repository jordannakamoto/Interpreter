// CST
// Concrete Syntax Tree

#ifndef CONCRETE_SYNTAX_TREE_H
#define CONCRETE_SYNTAX_TREE_H

#include <string>
#include "Token.h"

/* Concrete Syntax Tree Class Definition */

// All nodes are spawned pointing to null sibling -> and child v
class ConcreteSyntaxTree {
public:
    // :: Node :: 
    // Struct for single Node of Token Linked List
    struct Node {
        Node();
        Node(Token* t){ 
            token = t;
        };
        Token* token;                 // Token pointer
        Node* next_sibling = nullptr; // Horizontal sibling
        Node* next_child = nullptr;   // Vertical child

        // Getters
        Token* getToken() { return token; }
        Node* getNextSibling() { return next_sibling; }
        Node* getNextChild() { return next_child; }

        // Setters
        void setToken(Token* newToken) { token = newToken; }
        void setNextSibling(Node* newSibling) { next_sibling = newSibling; }
        void setNextChild(Node* newChild) { next_child = newChild; }
        ~Node() {}; // Token deletion handled by Token Linked List
    };

    // Constructor
    ConcreteSyntaxTree(){};

    // Attributes
    // head: Start of Token Linked List
    // tail: End of Token Linked List
    Node* head = nullptr; 
    Node* tail = nullptr; 
    int list_size = 0;
    
    // Functions
    
    // we only need to ever add
    // siblings or children at tail position
    void addNodeSibling(Token* token);
    void addNodeChild(Token* token);

    // Getters
    Node* getHead() { return head; };
    Node* getTail() { return tail; };
    int getListSize() { return list_size; };

    // Setters
    void setHead(Node* node){head = node;};
    void setTail(Node* node){tail = node;};

    // Print Functions
    void printList();
    void printListDebug();
    void printInstructions();

    // ---- Notes ----
    // ---------------
    ~ConcreteSyntaxTree();

};

#endif //CONCRETE_SYNTAX_TREE_H