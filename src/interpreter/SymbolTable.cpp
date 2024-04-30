#include<vector>
#include<unordered_set>
#include<iomanip>
#include"SymbolTable.h"

// STEntry
STEntry::STEntry(std::string name, id_type idtype, d_type dtype, bool isArray, int size, int scope, Token* value) {
    setIDName(name);
    setID_Type(idtype);
    setD_Type(dtype);
    setScope(scope);
    setArraySize(size);
    setIsArray(isArray);
    this->value = value;
}

void STEntry::setIDName(std::string name) {
    this->IDENTIFIER_NAME = name;
}

void STEntry::setID_Type(id_type type) {
    this->IDENTIFIER_TYPE = type;
}

void STEntry:: setD_Type(d_type type) {
    this->DATATYPE = type;
}

void STEntry::setScope(int scope) {
    this->SCOPE = scope;
}

void STEntry::setArraySize(int size) {
    this->DATATYPE_ARRAY_SIZE = size;
}

void STEntry::setIsArray(bool array) {
    this->DATATYPE_IS_ARRAY = array;
}

void STEntry::setValue(Token *val) {
    value = val;
}



// SymbolTable

SymbolTable::SymbolTable() {
}

SymbolTable::Node::Node(STEntry* e) {
    setEntry(e);
}

void SymbolTable::Node::printNode() {
    std::string isArray;
    if (entry->getIsArray()) {
        isArray = "yes";
    }
    else {
        isArray = "no";
    }
    std::cout << std::setw(21) << "IDENTIFIER_NAME" << ": " << entry->getIDName() << std::endl;
    std::cout << std::setw(21) << "IDENTIFIER_TYPE" << ": " << idVec[entry->getID_Type()] << std::endl;
    std::cout << std::setw(21) << "DATATYPE" << ": " << dVec[entry->getD_Type()] << std::endl;
    std::cout << std::setw(21) << "DATATYPE_IS_ARRAY" << ": " << isArray << std::endl;
    std::cout << std::setw(21) << "DATATYPE_ARRAY_SIZE" << ": " << entry->getArraySize() << std::endl;
    std::cout << std::setw(21) << "SCOPE" << ": " << entry->getScope() << std::endl;
    std::cout << std::setw(21) << "VALUE" << ": " << entry->getValue()->getTokenValue() << std::endl;
    std::cout << "\n";
}

void SymbolTable::Node::printParameterNode() {
    std::string isArray;
    if (entry->getIsArray()) {
        isArray = "yes";
    }
    else {
        isArray = "no";
    }
    std::cout << std::setw(21) << "IDENTIFIER_NAME" << ": " << entry->getIDName() << std::endl;
    std::cout << std::setw(21) << "DATATYPE" << ": " << dVec[entry->getD_Type()] << std::endl;
    std::cout << std::setw(21) << "DATATYPE_IS_ARRAY" << ": " << isArray << std::endl;
    std::cout << std::setw(21) << "DATATYPE_ARRAY_SIZE" << ": " << entry->getArraySize() << std::endl;
    std::cout << std::setw(21) << "SCOPE" << ": " << entry->getScope() << std::endl;
    std::cout << std::setw(21) << "VALUE" << ": " << entry->getValue()->getTokenValue() << std::endl;
    std::cout << "\n";
}

void SymbolTable::printParameterList(Node* n) {
    while (n != nullptr) {
        n->printParameterNode();
        n = n->getNext();
    }
}

// Searches table with 
SymbolTable::searchResults SymbolTable::searchSymbolTable(Node* head, Node* n){
    // If ST is empty
    if (head == nullptr) return none;

    
    // Load in data for node to search
    id_type     nType = n->getEntry()->getID_Type();
    std::string nName  = n->getEntry()->getIDName();
    int         nScope = n->getEntry()->getScope();
    searchResults        found;

    //std::cout << "st searched:" << std::endl;
    //std::cout << "nName" << " " << nName << std::endl;

    while (head != nullptr) {
        //std::cout << head->getEntry()->getIDName() << std::endl;
        /*
        if (nName == "found_something") {
            std::cout << "\n\n\n\nFOUND SOMETHING" << std::endl;
            std::cout << "iterator: " << std::endl;
            std::cout << head->getEntry()->getIDName() << std::endl;
            std::cout << head->getEntry()->getID_Type() << std::endl;
            std::cout << head->getEntry()->getScope() << std::endl;
            std::cout << "n:" << std::endl;
            std::cout << nName << std::endl;
            std::cout << nType << std::endl;
            std::cout << nScope << std::endl;

        }
        */

        // found a match
        if (head->getEntry()->getID_Type() == nType && 
            head->getEntry()->getIDName() == nName &&
            (head->getEntry()->getScope() == nScope ||
            head->getEntry()->getScope() == 0))
            {
            if (head->getEntry()->getScope() == 0) {
                return global;
            }
            else {
                return local;
            }
        }
        if (head->parameterList != nullptr) {
            found = searchSymbolTable(head->parameterList, n);
            if (found != none) {
                return found;
            }
        }
        head = head->getNext();
    }
    return none;
}

bool SymbolTable::searchSymbolTableFunctions(std::string n){
    // If ST is empty
    Node* h = this->getHead();
    if (h == nullptr) return none;

    while (h != nullptr) {
        // found a match
        if (h->getEntry()->getIDName() == n && h->getEntry()->getID_Type()==id_type::function)
        {
            return true;
        }
        h = h->getNext();
    }
    return false;
}


// SearchSymbolTableFunctionByScope
// Args:     n - scope number to search for
//
// returns:  pointer STEntry function/procedure at desired scope
//           nullptr if not found  
STEntry* SymbolTable::searchSymbolTableByScope(int n){
    // If ST is empty
    Node* h = this->getHead();
    if (h == nullptr) return nullptr;

    while (h != nullptr) {
        // found a match
        if (h->getEntry()->getScope() == n && (
        h->getEntry()->getID_Type()==id_type::function ||
        h->getEntry()->getID_Type()==id_type::procedure))
        {
            return h->getEntry();
        }
        h = h->getNext();
    }
    return nullptr;
}

// getVariablesByScope
// Args:     n - scope number to search for
//
// returns:  Variables + Parameters as STEntry* vector
//           of all elements in the current scope...
// > Assume parameters don't have their own namespace
std::vector<STEntry*> SymbolTable::getVariablesByScope(int n){
    Node* h = this->getHead();
    // early return If ST is empty
    if (h == nullptr) return std::vector<STEntry*>();

    std::vector<STEntry*> entries;
    while (h != nullptr) {
        // Gather Variables
        if(h->getEntry()->getScope() == n){
            if(h->getEntry()->getID_Type() == datatype){
                entries.push_back(h->getEntry());
            }
        // Gather Parameters
            else if(h->getEntry()->getID_Type() == function || h->getEntry()->getID_Type() == procedure){
                if (h->getEntry()->getScope() && h->getParameterList() != nullptr){
                    Node* n = h->getParameterList();
                    while (n != nullptr) {
                        entries.push_back(n->getEntry());
                        n = n->getNext();
                    }
                }
            }
        }
        h = h->getNext();
    }
    return entries;
}

std::vector<STEntry*> SymbolTable::getParametersByScope(int n){
    Node* h = this->getHead();
    // early return If ST is empty
    if (h == nullptr) return std::vector<STEntry*>();

    std::vector<STEntry*> entries;
    while (h != nullptr) {
        if(h->getEntry()->getScope() == n){
        // Gather Parameters
            if(h->getEntry()->getID_Type() == function || h->getEntry()->getID_Type() == procedure){
                if (h->getEntry()->getScope() && h->getParameterList() != nullptr){
                    Node* n = h->getParameterList();
                    while (n != nullptr) {
                        entries.push_back(n->getEntry());
                        n = n->getNext();
                    }
                }
            }
        }
        h = h->getNext();
    }
    return entries;
}


void SymbolTable::addNode(Node* node) {
    int searchValue = searchSymbolTable(this->getHead(), node);
    if (searchValue > none) {
            // build error string
            std::string errorMessage = "Error on line " + std::to_string(itr->getToken()->getTokenLineNumber()) +
            ": variable \"" + node->getEntry()->getIDName() + "\" is already defined " +
            sResults[searchValue] + "ly";
            // Output to stderr and file
            std::cerr << errorMessage << std::endl;
            ErrorHandler::get().outputErrorToFile(errorMessage);
        exit(-1);
    }
    if (head == nullptr) {
        head = node;
        tail = node;
    }
    else {
        tail->setNext(node);
        tail = node;
    }
}

void SymbolTable::printTable() {
    Node* tableITR = this->getHead();
    if (tableITR == nullptr) {
        std::cerr << "head is null..." << std::endl;
    }
    // print all table values
    while (tableITR != nullptr) {
        tableITR->printNode();
        tableITR = tableITR->getNext();
    }
    // std::cout << std::endl;

    tableITR = this->getHead();

    // then print the parameter lists at the bottom
    while (tableITR != nullptr) {
        // if a parameter list is found, print it out
        if (tableITR->getParameterList() != nullptr) {
            std::cout << "\n";
            std::cout << std::setw(21) << "PARAMETER LIST FOR" << ": " << tableITR->getEntry()->getIDName() << std::endl;
            printParameterList(tableITR->getParameterList());
        }
        tableITR = tableITR->getNext();
    }
}



bool SymbolTable::createSymbolTable(ConcreteSyntaxTree* cst) {
    //std::cout << "creating symbol table..." << std::endl;
    Token* tempToken = new Token("0", NONE, -1);

    itr = cst->getHead();
    if (itr == nullptr) {
        std::cerr << "cst head is null... " << std::endl;
        exit(-1);
    }
    lastScope = 0;
    currScope.push(lastScope);
    int count = 0;
    std::string tokenValue, dtValue, name;
    STEntry* tempEntry = nullptr;
    Node* listNode = nullptr;
    Node* tempNode = nullptr;
    while (itr != nullptr) {
        // std::cout << "CURRENT: " << itr->getToken()->getTokenValue() << " TYPE: " << itr->getToken()->getTokenType() << " LINE: " << itr->getToken()->getTokenLineNumber()<< std::endl;
        // std::cout << "SYMBOL TABLE: " << std::endl;  // DEBUG
        // printTable(); // DEBUG
        if (itr->getToken()->getTokenType() == tdfa::LEFT_BRACE) {
            //std::cout << "line: " << itr->getToken()->getTokenLineNumber() << " braces: " << braces << std::endl;
            braces++;
        }
        else if (itr->getToken()->getTokenType() == tdfa::RIGHT_BRACE) {
            //std::cout << "line: " << itr->getToken()->getTokenLineNumber() << " braces: " << braces << std::endl;
            braces--;
        }
        if (itr->getToken()->getTokenType() == tdfa::IDENTIFIER) {
            tokenValue = itr->getToken()->getTokenValue();
            // parsing a function/procedure, and adding to its parameter list
            if (tokenValue == "function") {
                // go to new scope
                lastScope++;
                currScope.push(lastScope);

                // get type of function, check if it's valid
                itr = itr->getNextSibling();
                if (itr == nullptr) {
                    std::cerr << "no type given for function/procedure" << std::endl;
                }
                dtValue = itr->getToken()->getTokenValue();
                if (datatypes.find(dtValue) == datatypes.end()) {
                    std::cerr << "no matching datatype for " << dtValue << std::endl;
                }

                // get the name of the function
                itr = itr->getNextSibling();
                if (itr == nullptr) {
                    std::cerr << "no name for function/procedure" << std::endl;
                }

                // function
                // TODO: Name gets set, have to see if name already exists in Symbol Table
                name = itr->getToken()->getTokenValue();

                // initialize new entry using the information we have
                tempEntry = new STEntry(name, idMap[tokenValue], dMap[dtValue], false, 0, currScope.top(), tempToken);
                tempNode  = new Node(tempEntry);
                this->addNode(tempNode);

                itr = itr->getNextSibling();
                if (itr->getToken()->getTokenType() != tdfa::LEFT_PARENTHESIS) {
                    std::cerr << "need parenthesis after function/procedure declaration" << std::endl;
                    exit(-5);
                }
                itr = itr->getNextSibling();
                // no parameters
                if (itr->getToken()->getTokenValue() == "void") {
                    //std::cout << "no parameters" << std::endl;  // DEBUG
                    itr = itr->getNextSibling();
                    continue;
                }
                else {
                    // TODO: need to create parameter list
                    //pList = new Node();
                    // parse the parameters
                    tokenValue = itr->getToken()->getTokenValue();
                    if (datatypes.find(tokenValue) == datatypes.end()) {
                        std::cerr << "invalid datatype" << std::endl;
                        exit(-1);
                    }
                    else {
                        listNode = parseParameters();
                        this->getTail()->setParameterList(listNode);
                        if (this->getTail()->getParameterList() == nullptr) {
                            std::cerr << "parameter list not being set" << std::endl;  // DEBUG
                        }
                        else {
                            //std::cout << "DEBUG printing parameter list" << std::endl;
                            //printParameterList(listNode);
                        }
                    }
                }
                if (itr->getNextSibling() != nullptr) {
                    itr = itr->getNextSibling();
                    std::cerr << "brace required after function/procedure declaration" << std::endl;
                }
                else {
                    itr = itr->getNextChild();
                }
                // std::cout << "now " << itr->getToken()->getTokenValue() << std::endl;
            }
            else if (tokenValue == "procedure") {
                lastScope++;
                currScope.push(lastScope);

                // get procedure name
                itr = itr->getNextSibling();
                if (itr == nullptr) {
                    std::cerr << "no name for function/procedure" << std::endl;
                }
                // procedure
                // TODO: Name gets set, have to see if name already exists in Symbol Table
                name = itr->getToken()->getTokenValue();

                // initialize new entry
                tempEntry = new STEntry(name, idMap[tokenValue], d_type::NOT_APPLICABLE, false, 0, currScope.top(), tempToken);
                tempNode  = new Node(tempEntry);
                this->addNode(tempNode);

                // check if there are parameters for the procedure
                itr = itr->getNextSibling();
                if (itr->getToken()->getTokenType() != tdfa::LEFT_PARENTHESIS) {
                    std::cerr << "need parenthesis after function/procedure declaration" << std::endl;
                    exit(-5);
                }
                itr = itr->getNextSibling();
                // no parameters
                if (itr->getToken()->getTokenValue() == "void") {
                    //std::cout << "no parameters" << std::endl;  // DEBUG
                    itr = itr->getNextSibling();
                    continue;
                }
                else { // create parameter list by parsing parameters
                    tokenValue = itr->getToken()->getTokenValue();
                    if (datatypes.find(tokenValue) == datatypes.end()) {
                        std::cerr << "invalid datatype" << std::endl;
                        exit(-1);
                    }
                    else {
                        listNode = parseParameters();
                        this->getTail()->setParameterList(listNode);
                        if (this->getTail()->getParameterList() == nullptr) {
                            std::cerr << "parameter list not being set" << std::endl;  // DEBUG
                        }
                    }
                }
                if (itr->getNextSibling() != nullptr) {
                    //std::cout << "procedure getting next sibling" << std::endl;
                    itr = itr->getNextSibling();
                }
                else {
                    //std::cout << "procedure getting next child" << std::endl; // The program went to get the nect child but there was no indicating end brace in the symbol table
                    //std::cerr << "brace required after function/procedure declaration" << std::endl;
                    itr = itr->getNextChild();
                }
                //std::cout << "now " << itr->getToken()->getTokenType() << std::endl;
            }
            else if (datatypes.find(tokenValue) != datatypes.end()) { // it's a valid datatype
                // parse symbol
                // std::cout << "found valid datatype, parsing" << std::endl;
                //std::cout << "found: " << tokenValue << " , ";
                parseVariable();
                //std::cout << "finished parsing datatype" << std::endl;
            }
            else { // not a variable/function/procedure declaration
                if (itr->getNextSibling() == nullptr) {
                    itr = itr->getNextChild();
                }
                else {
                    itr = itr->getNextSibling();
                    //std::cout << "going to next sibling..." << std::endl;  // DEBUG
                }
            }
        }
        else {
            if (itr->getNextSibling() == nullptr) {
                itr = itr->getNextChild();
            }
            else {
                itr = itr->getNextSibling();
                //std::cout << "going to next sibling..." << std::endl;  // DEBUG
            }
        }
        count++;
        // std::cout << "\n\n\n\n";
    }
    return true;
}

// Parses a symbol, with error checks. will move the iterator to one after the symbol name.
void SymbolTable::parseVariable() {
    //std::cout << "parsing VARIABLE(S)" << std::endl;
    Token* tempToken = new Token("0", NONE, -1);
    std::string datatype, name;
    Node* tempNode = nullptr;
    STEntry* tempEntry = nullptr;
    bool isArray;
    int arraySize;

    datatype = itr->getToken()->getTokenValue();

    do {
        isArray   = false;
        arraySize = 0;
        itr       = itr->getNextSibling();
        if (itr == nullptr || itr->getToken()->getTokenType() != tdfa::IDENTIFIER) {
            std::cerr << "no name for variable declaration" << std::endl;
            if (itr != nullptr) {
                std::cerr << datatype << std::endl;
                std::cerr << itr->getToken()->getTokenValue() << std::endl;
            }
            else {
                std::cerr << "itr is null" << std::endl;
            }
            exit(-1);
        }

        // variable
        // TODO: Name gets set, have to see if name already exists in Symbol Table
        name = itr->getToken()->getTokenValue();

        itr = itr->getNextSibling();
        
        if (itr == nullptr) {
            std::cerr << "no sibling node after variable type" << std::endl;
            if (itr != nullptr) {
                std::cerr << datatype << std::endl;
                std::cerr << itr->getToken()->getTokenValue();
            }
            else {
                std::cerr << "itr is null" << std::endl;;
            }
            exit(-2);
        }
        else if (itr->getToken()->getTokenType() == tdfa::LEFT_BRACKET){ // array identified
            isArray = true;
            itr     = itr->getNextSibling();
            if (itr == nullptr || itr->getToken()->getTokenType() != tdfa::INTEGER) {
                std::cerr << "invalid size for array" << std::endl;
                if (itr != nullptr) {
                    std::cerr << itr->getToken()->getTokenValue();
                }
                else {
                    std::cerr << "itr is null" << std::endl;;
                }
                exit(-3);
            }
            else {
                arraySize = std::stoi(itr->getToken()->getTokenValue());
            }
            itr = itr->getNextSibling();
            // needs a closing bracket after array size
            if (itr->getToken()->getTokenType() != tdfa::RIGHT_BRACKET) {
                std::cerr << "needs a closing bracket after array size" << std::endl;
            }
            itr = itr->getNextSibling();
        }

        tempEntry = new STEntry(name, id_type::datatype, dMap[datatype], isArray, arraySize, currScope.top(), tempToken);
        if (braces == 0) {
            //std::cout << name << std::endl;
            //std::cout << "scope: 0" << std::endl;
            tempEntry->setScope(0);
        }
        else {
            // Scope counter
            //std::cout << name << std::endl;
            //std::cout << "scope: " << tempEntry->getScope() << std::endl;
        }
        tempNode  = new Node(tempEntry);
        // search for duplicates
        int searchValue = searchSymbolTable(this->getHead(), tempNode);
        if (searchValue > none) {
            std::string errorMessage = "Error on line " + std::to_string(itr->getToken()->getTokenLineNumber()) +
            ": variable \"" + name + "\" is already defined " + sResults[searchValue] + "ly";
            std::cerr << errorMessage;
            ErrorHandler::get().outputErrorToFile(errorMessage);
            exit(-1);
        }
        this->addNode(tempNode);

        //std::cout << "parsed name: " << name << std::endl;
    } while (itr->getToken()->getTokenType() == tdfa::COMMA);
}

// method to parse the parameters of a funciton/procedure
SymbolTable::Node* SymbolTable::parseParameters() {
    //std::cout << "Parsing PARAMETER(S)" << std::endl;
    Token* tempToken = new Token("0", NONE, -1);
    std::string datatype,                        name;
    //tdfa::Token_Type tokenType;
    bool isArray;
    int   arraySize          = 0;
    Node* parametersNodeHead = nullptr;
    Node* tempNode = nullptr;
    Node* itrNode = nullptr;
    STEntry* tempEntry = nullptr;

    do {
        isArray   = false;
        arraySize = 0;
        if (itr->getToken()->getTokenType() == tdfa::COMMA) {
            itr = itr->getNextSibling();
        }
        datatype = itr->getToken()->getTokenValue();
        //std::cout << "parameter datatype " << datatype << std::endl;
        //TODO: check datatype here

        itr = itr->getNextSibling();
        if (itr == nullptr || itr->getToken()->getTokenType() != tdfa::IDENTIFIER) {
            std::cerr << "no name for variable declaration" << std::endl;
            if (itr != nullptr) {
                std::cerr << datatype << std::endl;
                std::cerr << itr->getToken()->getTokenValue() << std::endl;
            }
            else {
                std::cerr << "itr is null" << std::endl;
            }
            exit(-1);
        }

        // parameters
        // TODO: Name gets set, have to see if name already exists in Symbol Table
        name = itr->getToken()->getTokenValue();

        itr = itr->getNextSibling();
        
        if (itr == nullptr) {
            std::cerr << "no sibling node after variable type" << std::endl;
            if (itr != nullptr) {
                std::cerr << datatype << std::endl;
                std::cerr << itr->getToken()->getTokenValue();
            }
            else {
                std::cerr << "itr is null" << std::endl;;
            }
            exit(-2);
        }
        else if (itr->getToken()->getTokenType() == tdfa::LEFT_BRACKET){ // array identified
            isArray = true;
            itr     = itr->getNextSibling();
            if (itr == nullptr || itr->getToken()->getTokenType() != tdfa::INTEGER) {
                std::cerr << "invalid size for array" << std::endl;
                if (itr != nullptr) {
                    std::cerr << itr->getToken()->getTokenValue();
                }
                else {
                    std::cerr << "itr is null" << std::endl;;
                }
                exit(-3);
            }
            else {
                arraySize = std::stoi(itr->getToken()->getTokenValue());
            }
            itr = itr->getNextSibling();
            // needs a closing bracket after array size
            if (itr->getToken()->getTokenType() != tdfa::RIGHT_BRACKET) {
                std::cerr << "needs a closing bracket after array size" << std::endl;
            }
            itr = itr->getNextSibling();
            if (itr == nullptr) {
                // required sibling
                exit(-1);
            }
        }

        tempEntry = new STEntry(name, id_type::datatype, dMap[datatype], isArray, arraySize, currScope.top(), tempToken);
        //std::cout << "parsed name: " << name << std::endl;
        tempNode = new Node(tempEntry);
        int searchValue = searchSymbolTable(this->getHead(), tempNode);
        if (searchValue > none) {
            std::string errorMessage = "Error on line " + std::to_string(itr->getToken()->getTokenLineNumber()) +
            ": variable \"" + name + "\" is already defined " + sResults[searchValue] + "ly";
            std::cerr << errorMessage << std::endl;
            ErrorHandler::get().outputErrorToFile(errorMessage);
            exit(-1);
        }
        // create another linked list
        if (parametersNodeHead == nullptr) {
            parametersNodeHead = tempNode;
            itrNode            = tempNode;
        }
        else {
            itrNode->setNext(tempNode);
            itrNode = tempNode;
        }
    } while (itr->getToken()->getTokenType() == tdfa::COMMA);
    // we have multiple
    //std::cout << "finished parsing parameters" << std::endl;
    if (parametersNodeHead == nullptr) {
        std::cerr << "parametersNodeHead is null..." << std::endl;
    }
    return parametersNodeHead;
}

STEntry *SymbolTable::lookupSymbol(const std::string &name, int scope) {
    Node* current = head;
    while (current != nullptr) {
        // Check if the current symbol matches the name and scope
        if (current->getEntry()->getIDName() == name && current->getEntry()->getScope() == scope) {
            // Return the symbol if found
            return current->getEntry();
        }
        // Move to the next symbol in the symbol table
        current = current->getNext();
    }
    // Return nullptr if the symbol is not found
    return nullptr;
}
