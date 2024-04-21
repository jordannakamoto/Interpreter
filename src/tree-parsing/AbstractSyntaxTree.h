// Created by Brayan Vizcaino on 4/3/24.


#ifndef ABSTRACTSYNTAXTREE_ABSTRACTSYNTAXTREE_HPP
#define ABSTRACTSYNTAXTREE_ABSTRACTSYNTAXTREE_HPP

#include <string>
#include <iostream>
#include <queue>
#include "ConcreteSyntaxTree.h"
#include "SymbolTable.h"
#include "Token.h"
#include "utilities/ShuntingYard.h"

using namespace std;

class AbstractSyntaxTree {
public:
    struct Node {
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
        ~Node() { delete token; };
    };
    // Constructor
    AbstractSyntaxTree();
    explicit AbstractSyntaxTree(ConcreteSyntaxTree* cst, SymbolTable* &st);
    ~AbstractSyntaxTree();

    AbstractSyntaxTree createAbstractSyntaxTree();
    Node* createDeclaration(Node* , AbstractSyntaxTree& ast);  //Creates a DECLARATION
    Node* createBlocks(Node* , AbstractSyntaxTree& ast);       //Creates a BEGIN/END Block
    Node* createAssignment(Node* , AbstractSyntaxTree& ast);  //Creates a ASSIGNMENT
    Node* createCondition(Node* , AbstractSyntaxTree& ast);    //Creates a IF/ELSE IF and WHILE/FOR
    Node* createReturn(Node* , AbstractSyntaxTree& ast);       //Creates a RETURN
    Node* createPrint(Node* , AbstractSyntaxTree& ast);        //Creates a PRINT
    Node* createFuncCall(Node* , AbstractSyntaxTree& ast);     //Creates a function CALL

    //TODO: The function below needs to written. All it needs to do is Convert Infix -> Postfix
    // and then store it into the Abstract Syntax Tree.
    Node* convertsInfixToPostfix(Node* , AbstractSyntaxTree& ast); //Converts Infix -> Postfix


    // Attributes
    // head: Start of Token Linked List
    // tail: End of Token Linked List
    Node* head = nullptr;
    Node* tail = nullptr;
    int list_size = 1;

    // FUNCTIONS
    // we only need to ever add siblings or children at tail position
    void addNodeSibling(Token *token);
    void addNodeChild(Token *token);

    void printAST();
    void printASTFormatted();
    void printInstructions();
    SymbolTable* localST = nullptr;

private:
};

#endif //ABSTRACTSYNTAXTREE_ABSTRACTSYNTAXTREE_HPP
