#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE
#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<unordered_set>
#include<stack>
#include "ConcreteSyntaxTree.h"
#include"ErrorHandler.h"

enum id_type {
    function,
    datatype,
    procedure,
};

enum d_type {
    d_bool,
    d_char,
    d_int,
    d_short,
    d_double,
    NOT_APPLICABLE,
};

const std::vector<std::string> idVec {"function", "datatype", "procedure"};
const std::vector<std::string> dVec {"bool", "char", "int", "short", "double", "NOT APPLICABLE"};
const std::unordered_set<std::string> datatypes {"bool", "char", "int", "short", "double", "void" };

class STEntry {
private:

    std::string IDENTIFIER_NAME;
    id_type IDENTIFIER_TYPE;
    d_type DATATYPE;
    bool DATATYPE_IS_ARRAY;
    int DATATYPE_ARRAY_SIZE;
    int SCOPE;
    Token* value;
    
public:
    STEntry(){};
    STEntry(std::string name, id_type idtype, d_type dtype, bool isArray, int size, int scope, Token* value);

    // setters
    void setIDName(std::string name);
    void setID_Type(id_type type);
    void setD_Type(d_type type);
    void setIsArray(bool array);
    void setArraySize(int size);
    void setScope(int scope);
    void setValue(Token* val);

    // getters
    std::string getIDName() { return IDENTIFIER_NAME; };
    id_type getID_Type() { return IDENTIFIER_TYPE; };
    d_type getD_Type() { return DATATYPE; };
    bool getIsArray() { return DATATYPE_IS_ARRAY; };
    int getArraySize() { return DATATYPE_ARRAY_SIZE; };
    int getScope() { return SCOPE; };
    Token* getValue() { return value; };
    std::string getSTValue() { return value->getTokenValue(); };
};


class SymbolTable {
public:
    struct Node {
        STEntry* entry = nullptr;
        Node* next = nullptr;
        Node* parameterList = nullptr;

        Node();
        Node(STEntry* e);

        // get/set entry
        STEntry* getEntry() { return entry; };
        void setEntry(STEntry* e) { entry = e; };

        // get/set next
        Node* getNext() { return next; };
        void setNext(Node* e) { next = e; };

        // get/set parameters list
        Node* getParameterList() { return parameterList; };
        void setParameterList(Node* e) { parameterList = e; };

        void printNode();
        void printParameterNode();

        ~Node() { delete entry; };
    };

    SymbolTable();

    Node* getHead() { return head; };
    Node* getTail() { return tail; };
    void addNode(Node* node);

    void printTable();
    void printParameterList(Node* n);

    bool createSymbolTable(ConcreteSyntaxTree* cst);
    void parseVariable();
    Node* parseParameters();
    enum searchResults {
        none,
        global,
        local,
    };

    searchResults searchSymbolTable(Node* head,Node* n);
    bool searchSymbolTableFunctions(std::string n);
    STEntry* searchSymbolTableByScope(int n);
    std::vector<STEntry*> getVariablesByScope(int n);
    std::vector<STEntry*> getParametersByScope(int n);
    STEntry* lookupSymbol(const std::string& name, int scope);



private:
    Node* head = nullptr;
    Node* tail = nullptr;

    int lastScope;
    std::stack<int> currScope;
    // increment by 1 if we have left brace, decrement by 2 if we have right brace
    int braces = 0;
    
    ConcreteSyntaxTree::Node* itr = nullptr;
    
    std::vector<std::string> sResults = {"none", "global", "local"};
    std::map<std::string, d_type> dMap = {{"bool", d_bool}, { "char", d_char}, {"int", d_int}, 
        {"short", d_short}, {"double", d_double}};

    //  std::map<std::string, id_type> idMap = {{"function", function}, {"datatype", datatype}, {"procedure", procedure}};


    //  I changed this line of code because I was getting a compiler error because I needed to use id_type to clearly specify where I get the function
    std::map<std::string, id_type> idMap = {{"function", id_type::function}, {"datatype", datatype}, {"procedure", procedure}};//I changed this line of code because I was getting a compiler error because I needed to use id_type to clearly specify where I get the function


};

#endif

