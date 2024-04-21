#ifndef SHUNTING_YARD
#define SHUNTING_YARD

#include <stack>
#include "ConcreteSyntaxTree.h"


using cNode = ConcreteSyntaxTree::Node;
class ShuntingYard {
private:
    std::stack<ConcreteSyntaxTree::Node*> operators;
    cNode* head = nullptr;
    cNode* tail = nullptr;
    cNode* output = nullptr;

public:
    ConcreteSyntaxTree::Node* inToPost(ConcreteSyntaxTree::Node*);
    void UnitTest();
    void print();
    static int isGreaterPrecedence(tdfa::Token_Type t1, tdfa::Token_Type t2);
    static bool isOperator(tdfa::Token_Type t1);
    static bool isNumericalOperator(tdfa::Token_Type t1);
    static bool isBooleanOperator(tdfa::Token_Type t1);

};

#endif

