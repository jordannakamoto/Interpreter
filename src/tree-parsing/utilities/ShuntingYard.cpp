#include <stack>
#include <iostream>
#include "ShuntingYard.h"

using cNode = ConcreteSyntaxTree::Node;

bool ShuntingYard::isOperator(tdfa::Token_Type t1) {
    return (isNumericalOperator(t1) || isBooleanOperator(t1));
};
bool ShuntingYard::isNumericalOperator(tdfa::Token_Type t1) {
    switch (t1) {
        case tdfa::PLUS:
        case tdfa::MINUS:
        case tdfa::ASTERISK:
        case tdfa::DIVIDE: 
        case tdfa::MODULO:
        case tdfa::CARET:
        case tdfa::ASSIGNMENT_OPERATOR:
            return true;
        default:
            return false;
    }
};

bool ShuntingYard::isBooleanOperator(tdfa::Token_Type t1) {
    switch (t1) {
        case tdfa::LT:
        case tdfa::GT:
        case tdfa::LT_EQUAL:
        case tdfa::GT_EQUAL:
        case tdfa::BOOLEAN_AND:
        case tdfa::BOOLEAN_OR:
        case tdfa::BOOLEAN_NOT:
        case tdfa::BOOLEAN_EQUAL:
        case tdfa::BOOLEAN_NOT_EQUAL:
            return true;
        default:
            return false;
    }
};

// determines if operator 1 of greater precedence than operator 2
// negative, t2 is strictly greater precedence
// positive, t1 is strictly greater precedence
int ShuntingYard::isGreaterPrecedence(tdfa::Token_Type t1, tdfa::Token_Type t2) {
    int precedence = 0;
    switch (t1) {
        case tdfa::ASSIGNMENT_OPERATOR:
            precedence = 0;
            break;
        case tdfa::BOOLEAN_OR:
            precedence = 1;
            break;
        case tdfa::BOOLEAN_AND:
            precedence = 2;
            break;
        case tdfa::BOOLEAN_EQUAL:
            precedence = 3;
            break;
        case tdfa::GT:
        case tdfa::LT:
        case tdfa::GT_EQUAL:
        case tdfa::LT_EQUAL:
            precedence = 4;
            break;
        case tdfa::PLUS: 
        case tdfa::MINUS:
            precedence = 5;
            break;
        case tdfa::ASTERISK:
        case tdfa::DIVIDE: 
        case tdfa::MODULO:
            precedence = 6;
            break;
        case tdfa::BOOLEAN_NOT:
            precedence = 7;
            break;
        default:
            precedence = -1;
            break;
    }
    switch (t2) {
        case tdfa::ASSIGNMENT_OPERATOR:
            precedence -= 0;
            break;
        case tdfa::BOOLEAN_OR:
            precedence -= 1;
            break;
        case tdfa::BOOLEAN_AND:
            precedence -= 2;
            break;
        case tdfa::BOOLEAN_EQUAL:
            precedence -= 3;
            break;
        case tdfa::GT:
        case tdfa::LT:
        case tdfa::GT_EQUAL:
        case tdfa::LT_EQUAL:
            precedence -= 4;
            break;
        case tdfa::PLUS: 
        case tdfa::MINUS:
            precedence -= 5;
            break;
        case tdfa::ASTERISK:
        case tdfa::DIVIDE: 
        case tdfa::MODULO:
            precedence -= 6;
            break;
        case tdfa::BOOLEAN_NOT:
            precedence -= 7;
            break;
        default:
            precedence = -1;
            break;
    }
    if (precedence == -1) {
        std::cerr << "operator of invalid type" << std::endl;
        exit(-1);
    }
    else {
        return precedence;
    }
};

ConcreteSyntaxTree::Node* ShuntingYard::inToPost(ConcreteSyntaxTree::Node* n) {
    std::stack<ConcreteSyntaxTree::Node*> s;
    ConcreteSyntaxTree::Node* itr = n;
    ConcreteSyntaxTree::Node* temp;
    auto type = tdfa::NONE;

    
    std::cout << "starting infix to postfix" << std::endl;
    // stop on delimiter for a statement
    while (itr->getToken()->getTokenType() != tdfa::SEMICOLON) {
        type = itr->getToken()->getTokenType();
        if (type == tdfa::IDENTIFIER) {
            std::cout << "number " << itr->getToken()->getTokenValue() << std::endl;
            // add to the output queue
            if (head == nullptr) {
                std::cout << "setting new head" << std::endl;
                head = itr;
                tail = itr;
            }
            else {
                tail->setNextSibling(itr);
                tail = tail->getNextSibling();
            }
            temp = itr;
            itr = itr->getNextSibling();
            temp->setNextSibling(nullptr);
        }
        else if (isOperator(type)) {
            std::cout << "operator: " << itr->getToken()->getTokenValue() << std::endl;
            // if precedence is less than current operator
            if (!operators.empty()) {
                int precedence = isGreaterPrecedence(operators.top()->getToken()->getTokenType(), type);
                while ((type == tdfa::CARET && precedence < 0) || precedence <= 0) {
                    std::cout << "pushed operator to output" << std::endl;
                    temp = operators.top();
                    tail->setNextSibling(temp);
                    tail = tail->getNextSibling();
                    operators.pop();
                }
            }
            std::cout << "pushed operator to the stack..." << std::endl;
            operators.push(itr);
            temp = itr;
            itr = itr->getNextSibling();
            temp->setNextSibling(nullptr);
        }
        else if (type == tdfa::LEFT_PARENTHESIS) {
            if (head == nullptr) {
                head = itr;
                tail = itr;
            }
            else {
                tail->setNextSibling(itr);
            }
            temp = itr;
            itr = itr->getNextSibling();
            temp->setNextSibling(nullptr);
        }
        else if (type == tdfa::RIGHT_PARENTHESIS) {
            while(operators.top()->getToken()->getTokenType() != tdfa::LEFT_PARENTHESIS) {
                temp = operators.top();
                tail->setNextSibling(itr);
                tail = tail->getNextSibling();
                operators.pop();
            }
            operators.pop(); // pop the left parenthesis too
            temp = itr;
            itr = itr->getNextSibling();
            temp->setNextSibling(nullptr);
        }
        else {
            std::cerr << "something went wrong in shunting yard" << std::endl;
            exit(-1);
        }
    }
    while (!operators.empty()) {
        temp = operators.top();
        tail->setNextSibling(temp);
        tail = tail->getNextSibling();
        operators.pop();
    }
    output = head;
    return output;
}

void ShuntingYard::print() {
    std::cout << "starting to print" << std::endl;
    cNode* temp;
    if (output == nullptr) {
        std::cout << "output is empty" << std::endl;
        return;
    }
    temp = output;
    while (temp != nullptr) {
        std::cout << temp->getToken()->getTokenValue() << " ";
        temp = temp->getNextSibling();
    }
    std::cout << std::endl;
    return;
}

void ShuntingYard::UnitTest() {
    // building blocks
    Token* num1 = new Token("3", tdfa::IDENTIFIER, 1);
    Token* num2 = new Token("5", tdfa::IDENTIFIER, 1);
    Token* lp = new Token("(", tdfa::LEFT_PARENTHESIS, 1);
    Token* rp = new Token(")", tdfa::RIGHT_PARENTHESIS, 1);
    Token* multiply = new Token("*", tdfa::ASTERISK, 1);
    Token* add = new Token("+", tdfa::PLUS, 1);
    Token* divide = new Token("/", tdfa::DIVIDE, 1);
    Token* caret = new Token("^", tdfa::CARET, 1);
    Token* semicolon = new Token(";", tdfa::SEMICOLON, 1);

    ConcreteSyntaxTree::Node* head = new ConcreteSyntaxTree::Node();
    ConcreteSyntaxTree::Node* temp;
    // 3 + 5;
    head->setToken(num1->copy());
    temp = head;
    temp->setNextSibling(new cNode(add->copy()));
    temp = temp->getNextSibling();
    temp->setNextSibling(new cNode(num2->copy()));
    temp = temp->getNextSibling();
    temp->setNextSibling(new cNode(semicolon->copy()));
    temp = temp->getNextSibling();
    temp = inToPost(head);
    std::cout << "finished constructing..." << std::endl;
    print();
}
