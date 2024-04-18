#include "ConcreteSyntaxTree.h"
#include <iostream>
#include <iomanip>

/* Concrete Syntax Tree Class Implementation */
ConcreteSyntaxTree::Node::Node() {
}

// Add
void ConcreteSyntaxTree::addNodeSibling(Token* token) {
    // 1. Make new Node
    // 2. Set new Node's token
    // 3. Connect new Node to List via tail.next_sibling
    // 4. set new tail and increment list_size

    Node* newNode = new Node();
    newNode->setToken(token);
    if (tail) {
        tail->setNextSibling(newNode);
        tail = newNode;
        list_size++;    
    } else { // if uninitialized, spawn at head
        head = newNode;
        tail = newNode;
        list_size = 1;
    }
}

void ConcreteSyntaxTree::addNodeChild(Token* token) {
    // Same method as above, but we are adding a child

    Node* newNode = new Node();
    newNode->setToken(token);
    if (tail) {
        tail->setNextChild(newNode);
        tail = newNode;
        list_size++;
    } else { // if uninitialized, spawn at head
        head = newNode;
        tail = newNode;
        list_size = 1;
    }
}

// Print
// Print the entire Token Linked List
// - Siblings inline separated by a space
// - Children on new lines
void ConcreteSyntaxTree::printList(){
    std::cout << "\nPrinting List __________________________________________\n" << std::endl;
    Node* current = head;
    while (current != nullptr) {

        // Print current node's token value
        std::cout << current->getToken()->getTokenValue();
        if (current->getNextSibling() != nullptr) {
            // Move to the next sibling if there is one
            std::cout << " ";
            current = current->getNextSibling(); 
        } else {
            // If this is the last sibling, move to its child
            current = current->getNextChild();
            std::cout << "\n"; // Indicate moving down a level
        }
    } // Exit while loop when current is null
    // std::cout << "\nList Printed..." << std::endl;
}

// Print Debug
// Print the entire Token Linked List
// - Siblings on newlines
// - Children noted by down arrow
void ConcreteSyntaxTree::printListDebug(){
    std::cout << "\nPrinting List Debug _____________________________________\n" << std::endl;
    Node* current = head;
    int lineNum = 1;
    std::cout << "line " << lineNum << std::endl;
    while (current != nullptr) {
        // Calculate the width needed to make the total output 15 characters
        // 30 characters total - 2 for ": " - length of the token type string
        int width = 18;

        std::cout << std::setw(width) << std::left << current->getToken()->getTokenTypeString() << ":  ";
        std::cout << current->getToken()->getTokenValue() << "\n";

        if (current->getNextSibling() != nullptr) {
            // Move to the next sibling if there is one
            current = current->getNextSibling(); 
        } else {
            // If this is the last sibling, move to its child
            current = current->getNextChild();
            if(current!= nullptr) {
                std::cout << "\n"; // Indicate moving down a level
                std::cout << "line " << ++lineNum << std::endl;
            }
        }
    } // Exit while loop when current is null
    std::cout << "\n";
    // std::cout << "\nList Printed..." << std::endl;
}

// Print instructions on how the tree is printed
void ConcreteSyntaxTree::printInstructions(){
    std::cout << "\nHOW TO READ OUTPUT: " << std::endl;
    std::cout << "Siblings are on the same line separated by space. Children of the last sibling are on the next line." << std::endl;
    std::cout << "\nOUT FILE INFORMATION:" << std::endl;
}

// Destructor
// Special traversal for the ConcreteSyntaxTree
// where child nodes exist only at the end of the sibling chain
ConcreteSyntaxTree::~ConcreteSyntaxTree() {
    Node* current = head;
    Node* temp;
    while (current != nullptr) {
        temp = current; // Prepare to delete the current node
        if (current->getNextSibling() != nullptr) {
            // If there is a next sibling, move to it...
            current = current->getNextSibling();
        } else {
            // If this is the last sibling, move to its child
            current = current->getNextChild();
        }
        delete temp; // Delete the node after moving 'current' to the next node in the traversal order
    } // Exit while loop when current is null
}

// ------------------------------------------------------------------------------------------
