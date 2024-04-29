// Created by Brayan Vizcaino on 4/3/24.
// Edited by others on 4/14/24

#include "AbstractSyntaxTree.h"
#include<stack>

AbstractSyntaxTree::AbstractSyntaxTree(){
    head = nullptr;
    tail = nullptr;
    list_size = 1;
}

//Overloaded Constructor
AbstractSyntaxTree::AbstractSyntaxTree(ConcreteSyntaxTree* cst, SymbolTable* &st) {
    ConcreteSyntaxTree* newTree = cst;

    this->head = reinterpret_cast<Node *>(newTree->head);
    this->tail = reinterpret_cast<Node *>(newTree->tail);
    this->list_size = newTree->list_size;

    if (st == nullptr) {
        std::cerr << "require symbol table" << std::endl;
        exit (-1);
    }
    this->localST = st;
    if (this->localST == nullptr) {
        std::cerr << "require symbol table" << std::endl;
        exit (-1);
    }
}

//Destructor
AbstractSyntaxTree::~AbstractSyntaxTree()= default;

// Main Procedure
// Create Abstract Syntax Tree
AbstractSyntaxTree AbstractSyntaxTree::createAbstractSyntaxTree() {
    AbstractSyntaxTree abstractSyntaxTree;
    abstractSyntaxTree.localST = this->localST;
    Node *astHead = this->head;
    Node *prevHead = this->head;
    Node *astTail = this->tail;

    // Important ---------------------------------------------//
    // Because our Head/Tail pointers are typecasts from CST Nodes,
    // We have to build our finished AST by... 
    // first creating new Tokens with desired properties
    // from the original cast CST::Node Token at astHead for example
    // then calling addSibling/addChild(our new Token)
    // to construct the result abstractSyntaxTree
    // in separate memory from the original CST Nodes!
    // -------------------------------------------------------//

    string tokenValue;

    while (astHead != astTail) {
        tokenValue = astHead->getToken()->getTokenValue();//Grabs Value of TokenValue from head*
        if (tokenValue == "function" || tokenValue == "procedure" || tokenValue == "void"
        || tokenValue == "int" || tokenValue == "char" || tokenValue == "bool") {//Creates DECLARATION
            astHead = createDeclaration(astHead, abstractSyntaxTree);
            continue;
        }
        else if (tokenValue == "{" || tokenValue == "}"){//Creates BEGIN/END BLOCK
            astHead = createBlocks(astHead, abstractSyntaxTree);
            continue;
        }
        else if (tokenValue == "="){//Creates ASSIGNMENT
            astHead = createAssignment(prevHead, abstractSyntaxTree);
            continue;
        }
        else if(tokenValue == "if" || tokenValue == "else" || tokenValue == "while"
                || tokenValue == "for"){ //Creates IF/WHILE Condition
            astHead = createCondition(astHead, abstractSyntaxTree);
            continue;
        }
        else if(tokenValue == "return"){ //Creates RETURN
            astHead = createReturn(astHead, abstractSyntaxTree);
            continue;
        }
        else if(tokenValue == "printf"){ //Creates PRINTF
            astHead = createPrint(astHead, abstractSyntaxTree);
            continue;
        }
        else if(tokenValue == "("){
            astHead = createFuncCall(prevHead, abstractSyntaxTree);
            continue;
        }

        //Continues Reading every Node in Linked List
        if(astHead->getNextSibling() != nullptr){
            if (prevHead->getToken()->getTokenType() != tdfa::IDENTIFIER) {
                prevHead = astHead;
            }
            astHead = astHead->getNextSibling();

        }
        else if(astHead->getNextChild() != nullptr){
            prevHead = astHead;
            astHead = astHead->getNextChild();
        }
        else{
            cerr << "ERROR: SHOULD NOT REACH THIS ELSE-STATEMENT" << endl;
            exit(10);
        }
    }

    //Needs this because final END BLOCK of file does not register in while loop
    // (while loop terminates at astTail)
    if(astTail->getToken()->getTokenValue() == "}"){
        Token* endBlockToken = new Token("END BLOCK", AST_END_BLOCK, -1);
        abstractSyntaxTree.addNodeChild(endBlockToken);
    }
    // else Assume the last token is a part of a declaration and we can just return without closing a block
    // since createDeclaration has already ran above
    
    return abstractSyntaxTree;
}


AbstractSyntaxTree::Node *AbstractSyntaxTree::createDeclaration(AbstractSyntaxTree::Node *astHead, AbstractSyntaxTree& ast) {
    Token* decToken;
    string tokenValue = astHead->getToken()->getTokenValue();

    // Added 4/17 - give this func/proc token a type so we can locate it later
    if (tokenValue == "function"){
        decToken = new Token("DECLARATION", AST_FUNCTION_DECLARATION, -1); // Initialize a Token object
    }
    else if(tokenValue == "procedure"){
        decToken = new Token("DECLARATION", AST_PROCEDURE_DECLARATION, -1);
    }
    else{
        decToken = new Token("DECLARATION", AST_VARIABLE_DECLARATION, -1); // Initialize a Token object
    }

    // Note: I don't think void is a datatype in the language, it's explicitly bound to procedure main -Jordan
    if(tokenValue == "function" || tokenValue == "procedure" || tokenValue == "void"){
        while (astHead->getNextSibling() != nullptr) {//Moves head to Next Line or Next Available Child Node
            astHead = astHead->getNextSibling();

        }
        ast.addNodeChild(decToken);

        return astHead;
    }

    else if(tokenValue == "int" || tokenValue == "char" || tokenValue == "bool"){
        int counter = 1;//Counts how many DECLARATION for ex: int i, j; needs 2 DECLARATIONs

        while (astHead->getNextSibling() != nullptr){//Moves head to Next Line or Next Available Child Node
            if(astHead->getToken()->getTokenValue() == ","){
                counter++;
            }
            astHead = astHead->getNextSibling();
        }

        for(int i = 0; i < counter; i++){
            ast.addNodeChild(decToken);
        }
        return astHead;
    }

    else{
        cerr << "ERROR: SHOULD NOT REACH THIS ELSE-STATEMENT" << endl;
        exit(12);
    }

}


AbstractSyntaxTree::Node *AbstractSyntaxTree::createBlocks(AbstractSyntaxTree::Node *astHead, AbstractSyntaxTree& ast) {
    // removed token instantiation from here because only one is added to the tree
    // meaning the other won't be cleaned up

    string tempAstHead = astHead->getToken()->getTokenValue();
    if(tempAstHead == "{"){
        ast.addNodeChild(new Token("BEGIN BLOCK", AST_BEGIN_BLOCK, -1));
        astHead = astHead->getNextChild();
    }
    else if(tempAstHead == "}"){
        ast.addNodeChild(new Token("END BLOCK", AST_END_BLOCK, -1));
        astHead = astHead->getNextChild();
    }
    else{
        cerr << "ERROR: SHOULD NOT REACH THIS ELSE-STATEMENT" << endl;
        exit(10);
    }
    return astHead;
}

AbstractSyntaxTree::Node *AbstractSyntaxTree::createAssignment(AbstractSyntaxTree::Node *prevASTHead, AbstractSyntaxTree& ast) {
    Token* assignmentToken = new Token("ASSIGNMENT", AST_ASSIGNMENT, -1);
    ast.addNodeChild(assignmentToken);

    prevASTHead = ast.convertsInfixToPostfix(prevASTHead,ast);//Creates Infix -> Postfix and stores it into the Abstract Syntax Tree. Then returns new Head Pointer
    while (prevASTHead->getToken()->getTokenValue() != ";"){//Loop Prints Infix
        prevASTHead = prevASTHead->getNextSibling();
    }

    return prevASTHead;
}

AbstractSyntaxTree::Node* AbstractSyntaxTree::createCondition(AbstractSyntaxTree::Node *astHead, AbstractSyntaxTree& ast) {

    if(astHead->getToken()->getTokenValue() == "if") {
        Token* ifToken = new Token("IF", AST_IF, -1);
        ast.addNodeChild(ifToken);

        astHead = astHead->getNextSibling()->getNextSibling();

        astHead = ast.convertsInfixToPostfix(astHead,ast);//Creates Infix -> Postfix and stores it into the Abstract Syntax Tree. Then returns new Head Pointer
        while (astHead->getNextSibling() != nullptr) {//Loop Prints Infix
            astHead = astHead->getNextSibling();
        }

        return astHead;
    }

    else if(astHead->getToken()->getTokenValue() == "else"){
        Token* elseToken = new Token("ELSE", AST_ELSE, -1);
        ast.addNodeChild(elseToken);

        astHead = astHead->getNextChild();
        return astHead;
    }

    else if(astHead->getToken()->getTokenValue() == "while"){
        Token* whileToken = new Token("WHILE", AST_WHILE, -1);
        ast.addNodeChild(whileToken);

        astHead = astHead->getNextSibling()->getNextSibling();

        astHead = ast.convertsInfixToPostfix(astHead,ast);//Creates Infix -> Postfix and stores it into the Abstract Syntax Tree. Then returns new Head Pointer
        while(astHead->getNextSibling() != nullptr){//Loop Prints Infix
            astHead = astHead->getNextSibling();
        }

        return astHead;
    }

    else if(astHead->getToken()->getTokenValue() == "for"){
        // Token* forExpressionToken = new Token("FOR EXPRESSION", NONE, -1);
        astHead = astHead->getNextSibling()->getNextSibling();

        for(int i = 1; i <= 3; i++){//Splits For Loop into 3 FOR LOOP EXPRESSIONS
            //ADDS "FOR EXPRESSION + i(Sibling Number)" CHILD!
            Token* forExpressionToken = new Token("FOR EXPRESSION " + std::to_string(i), AST_FOR, -1);
            ast.addNodeChild(forExpressionToken);

            astHead = ast.convertsInfixToPostfix(astHead,ast);//Creates Infix -> Postfix and stores it into the Abstract Syntax Tree. Then returns new Head Pointer
            while(astHead->getNextSibling() != nullptr){
                if(astHead->getToken()->getTokenValue() == ";"){
                    astHead = astHead->getNextSibling();
                    break;
                }
                astHead = astHead->getNextSibling();
            }
        }
        return astHead;
    }

    else{
        cerr << "ERROR: SHOULD NOT REACH THIS ELSE-STATEMENT" << endl;
        exit(11);
    }
}

AbstractSyntaxTree::Node *AbstractSyntaxTree::createReturn(AbstractSyntaxTree::Node *astHead, AbstractSyntaxTree& ast) {
    Token* returnToken = new Token("RETURN", AST_RETURN, -1);
    ast.addNodeChild(returnToken);

    astHead = astHead->getNextSibling();//Goes 1 right from return Node

    astHead = ast.convertsInfixToPostfix(astHead,ast);//Creates Infix -> Postfix and stores it into the Abstract Syntax Tree. Then returns new Head Pointer
    while (astHead->getToken()->getTokenValue() != ";"){//Loop Prints Infix
        astHead = astHead->getNextSibling();
    }
    return astHead;
}

AbstractSyntaxTree::Node *AbstractSyntaxTree::createPrint(AbstractSyntaxTree::Node *astHead, AbstractSyntaxTree& ast) {
    Token* printfToken = new Token("PRINTF", AST_PRINTF, -1);
    ast.addNodeChild(printfToken);

    astHead = astHead->getNextSibling();//Goes 1 right from printf Node

    // Parse the print statement by getNextSibling()...
    // Print Item can be STRING, IDENTIFIER, or INTEGER
    while (astHead->getToken()->getTokenValue() != ";"){// Break at end of statement
        Token* curr = astHead->getToken();
        if(curr->getTokenType() == STRING ||
        curr->getTokenType() == IDENTIFIER ||
        curr->getTokenType() == INTEGER){
            // Append and print the item
            ast.addNodeSibling(curr->copy());
        }
        astHead = astHead->getNextSibling();
    }
    return astHead;
}

AbstractSyntaxTree::Node *AbstractSyntaxTree::createFuncCall(AbstractSyntaxTree::Node *prevASTHead, AbstractSyntaxTree& ast) {
    Token* callToken = new Token("CALL", AST_CALL, -1);
    ast.addNodeChild(callToken);

    // Rule for passing function call and its parameters to AST
    // Assuming parameters are of types...
    // Just loop until ; and grab IDENTIFIER, INTEGER, or STRING
    while (prevASTHead->getToken()->getTokenValue() != ";"){
        Token_Type type = prevASTHead->getToken()->getTokenType();
        if( type == IDENTIFIER|| type== INTEGER || type == STRING){
            ast.addNodeSibling(prevASTHead->getToken()->copy());
        }
        prevASTHead = prevASTHead->getNextSibling();
    }
    return prevASTHead;
}

// ConvertsInfixToPostfix
// Operator precedence rules in ShuntingYard.cpp
// 
using sy = ShuntingYard;
AbstractSyntaxTree::Node *AbstractSyntaxTree::convertsInfixToPostfix(AbstractSyntaxTree::Node *astHead, AbstractSyntaxTree &ast) {

    AbstractSyntaxTree::Node* prevHead = astHead;
    std::stack<AbstractSyntaxTree::Node*> operators;
    AbstractSyntaxTree::Node* temp;
    tdfa::Token_Type type;
    bool string = false;
    while (astHead->getToken()->getTokenValue() != ";"){// Break at end of statement
        //if (string) {
        //    std::cout << "IN A STRING" << std::endl;
        //}
        //else {
        //    std::cout << "NOT IN A STRING" << std::endl;
        //}
        //std::cout << "working on " << astHead->getToken()->getTokenValue() << std::endl;
        type = astHead->getToken()->getTokenType();
        if (type == tdfa::IDENTIFIER || type == tdfa::WHOLE_NUMBER || type == tdfa::INTEGER || type == tdfa::CHARACTER || type == tdfa::STRING) {
            //std::cout << "number " << astHead->getToken()->getTokenValue() << std::endl;
            // in a function
            // add to the output queue
            if (localST->searchSymbolTableFunctions(astHead->getToken()->getTokenValue())) {
                while(astHead->getToken()->getTokenType() != tdfa::RIGHT_PARENTHESIS) {
                    ast.addNodeSibling(astHead->getToken()->copy());
                    astHead = astHead->getNextSibling();
                }
                // add the right parenthesis as well
                ast.addNodeSibling(astHead->getToken()->copy());
                astHead = astHead->getNextSibling();
            }
            else {
                ast.addNodeSibling(astHead->getToken()->copy());
                astHead = astHead->getNextSibling();
            }
        }
        else if (sy::isOperator(type)) {
            //std::cout << "operator: " << astHead->getToken()->getTokenValue() << std::endl;
            // if precedence is less than current operator
            if (!operators.empty()) {
                int precedence = sy::isGreaterPrecedence(operators.top()->getToken()->getTokenType(), type);
                while ((type == tdfa::CARET && precedence > 0) || precedence >= 0) {
                    temp = operators.top();
                    //std::cout << "pushed operator to output" << std::endl;
                    ast.addNodeSibling(temp->getToken()->copy());
                    operators.pop();
                    if (!operators.empty()) {
                        precedence = sy::isGreaterPrecedence(operators.top()->getToken()->getTokenType(), type);
                    }
                    else {
                        break;
                    }
                }
            }
            //std::cout << "pushed operator to the stack..." << std::endl;
            operators.push(astHead);
            astHead = astHead->getNextSibling();
        }
        else if (type == tdfa::LEFT_PARENTHESIS) {
            //std::cout << "left parenthesis, pushed to operators" << std::endl;
            operators.push(astHead);
            astHead = astHead->getNextSibling();
        }
        else if (type == tdfa::RIGHT_PARENTHESIS) {
            //std::cout << "right parenthesis, checking operators" << std::endl;
            // likely a comparison
            if (astHead->getNextSibling() == nullptr) {
                break;
            }
            while(!operators.empty() && operators.top()->getToken()->getTokenType() != tdfa::LEFT_PARENTHESIS) {
                temp = operators.top();
                ast.addNodeSibling(temp->getToken()->copy());
                operators.pop();
            }
            operators.pop(); // pop the left parenthesis too
            astHead = astHead->getNextSibling();
        }
        else if (type == tdfa::SINGLE_QUOTE || type == tdfa::DOUBLE_QUOTE) {
            string = !string;
            ast.addNodeSibling(astHead->getToken()->copy());
            astHead = astHead->getNextSibling();
        }
        else if (type == tdfa::ESCAPE_CHARACTER) {
            ast.addNodeSibling(astHead->getToken()->copy());
            astHead = astHead->getNextSibling();
        }
        else if (type == tdfa::LEFT_BRACKET) {
            // add the left bracket, the index, and right bracket
            ast.addNodeSibling(astHead->getToken()->copy());
            astHead = astHead->getNextSibling();
            ast.addNodeSibling(astHead->getToken()->copy());
            astHead = astHead->getNextSibling();
            ast.addNodeSibling(astHead->getToken()->copy());
            astHead = astHead->getNextSibling();
        }
        else { // ERROR HANDLING
            std::cerr << "something went wrong in shunting yard" << std::endl;
            std::cerr << astHead->getToken()->getTokenValue() << std::endl;
            std::cerr << astHead->getToken()->getTokenType() << std::endl;
            ast.printAST();
            exit(-1);
        }
    }
    while (!operators.empty()) {
        temp = operators.top();
        ast.addNodeSibling(temp->getToken());
        operators.pop();
    }

    //std::cout << "finished shunting yard..." << std::endl;
    //std::cout << "checking prevhead" << std::endl;
    //std::cout << "exiting..." << std::endl;
    //printAST();
    return prevHead;
}

void AbstractSyntaxTree::addNodeSibling(Token *token) {
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

void AbstractSyntaxTree::addNodeChild(Token *token) {
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

// Original PrintAST
void AbstractSyntaxTree::printAST() {
    cout << "\n\nPrinting AST (Breadth-First Order) =============================\n" << endl;
    if (!head) {
        cout << "Empty AST" << endl;
        return;
    }

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
    }
    cout << "\nAST Printed..." << endl;
}

void AbstractSyntaxTree::printASTFormatted(){

    cout << "\n\nPrinting AST (Breadth-First Order) =============================\n" << endl;
    if (!head) {
        cout << "Empty AST" << endl;
        return;
    }

    Node* current = head;

    // sib_counter used for syntax checking

    // 1. for CALL function calls, we have to insert the list after the name of the function
    //    i.e. `CALL sum_of_first_n_squares (`
    //          instead of `ASSIGNMENT (sum|2|=)
    //    so the list characters start printing after sib 2 instead of sib 1
    // 2. for lists of operands, operators, arguments, etc.
    //    we insert spacer characters `|` after every item except the last
    int sib_counter = 0;
    bool isFunctionCall = false;
    while(current != nullptr){
        // At the first token of a line, decide if we need to set the flag for CALL handling or not
        if(sib_counter == 0 ){
            (current->getToken()->getTokenValue() == "CALL") ? isFunctionCall = true : isFunctionCall = false;              
            }
        if(sib_counter == 1){
            std::cout << " ";
            if(!isFunctionCall){ // insert the start of list delimiter if not a function call
                std::cout << "(";
            }
        }
        if(sib_counter == 2  && isFunctionCall){
            std::cout << " ("; // otherwise insert the start of list delimiter at sib 2
        }
        std::cout << current->getToken()->getTokenValue();
        if (current->getNextSibling() != nullptr) {
            sib_counter++;
            // Insert list separators until last item in line
            // if isFunctionCall, start printing separators at sib3 instead of sib2
            if(sib_counter == 2 && !isFunctionCall && current->getNextChild() == nullptr){
                std::cout << "|";
            }
            if(sib_counter > 2 && current->getNextChild() == nullptr){
                std::cout << "|";
            }
            current = current->getNextSibling();
        } else {
            // If we were on a line with a list, close the list
            if (sib_counter >= 1 ){
                    std::cout << ")";
            }
            // If we reached EOF, don't print a newline
            if(current->getNextChild() != nullptr){ 
                std::cout << "\n"; // Otherwise for nextChild, indicate moving down a level with a newline
            }
            sib_counter = 0;
            current = current->getNextChild();
        }
    }
    std::cout << "\n\n";
}

void AbstractSyntaxTree::printInstructions(){
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "Print Format:" << std::endl;
    std::cout << "\tSiblings printed on same line" << std::endl;
    std::cout << "\t()  surrounds Operators/Operands/Arguments" << std::endl;
    std::cout << "\t|   separates Sibling Group Elements"<< std::endl;
    std::cout << "\tChildren of last element printed on next line" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
}


