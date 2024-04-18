// Written by Jacob Jaffe and Brayan Vizcaino
// Edited by Brayan Vizcaino Mar-25

#include "Tokenizer.h"

// main method, TokenizeToLinkedList() starts on line 70
// Constructor
// - Called in main.cpp. fileHandler passes its fileContent by argument. The content is copied into Tokenizer.fileStr
Tokenizer::Tokenizer(std::string fileContent) {
    i = 0;
    lineNumber = 1;
    fileStr = std::move(fileContent); 
}

// Detects if a char c is considered a single character token
// - Goes to start state since a single character token is fully formed
bool Tokenizer::isSingleToken(char c, TokenList* tl, Tokenizer_DFA_State& state) {
    switch(c) {
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}': 
        case '+':
        case '*':
        case '/':
        case '%':
        case ',':
        case ';':
            state = start;
            return true;
        default: 
            return false;
    }
}

// Detects if char is the start of a multi character token
// - Goes to corresponding initial state of the branch for that potentially muli-char token
bool Tokenizer::isPotentialToken(char c, TokenList* tl, Tokenizer_DFA_State& state) {
    switch (c) {
        case '<':
            state = lt;
        break;
        case '>':
            state = gt;
        break;
        case '-':
            state = negative;
        break;
        case '|':
            state = bool_or;
        break;
        case '&':
            state = bool_and;
        break;
        case '!':
            state = bool_not;
        break;
        case '=':
            state = assign;
        break;
        default: 
            return false;
    }
    return true;
}

// TokenizeToLinkedList
// pre-condition: Tokenizer::fileStr contains an appropriate file string to be tokenized
// returns:       TokenList* of all tokens that were parsed and created
//
TokenList* Tokenizer::TokenizeToLinkedList(){
    TokenList* tl = new TokenList();
    char c;
    Tokenizer_DFA_State state = start; // current state of the DFA. state enum defined in header file
    string temp;                       // stores temporary tokenValue. e.g. a string that is being built
    lineNumber = 1;
    int quoteStartLineNumber;
    i = 0;

    std::unordered_map<char, int> mp = {
        {'(', LEFT_PARENTHESIS},
        {')', RIGHT_PARENTHESIS},
        {'[', LEFT_BRACKET},
        {']', RIGHT_BRACKET},
        {'{', LEFT_BRACE},
        {'}', RIGHT_BRACE},
        {'+', PLUS},
        {'%', MODULO},
        {'*', ASTERISK},
        {'/', DIVIDE},
        {',', COMMA},
        {';', SEMICOLON},
    };

    while(i < fileStr.length()) {
        c = fileStr.at(i);
        if (c == '\n') {
            lineNumber++;
            i++;
            continue;
        }
        switch(state) {
            case start: 
                if (c == '\'') {
                    state = single_quote;
                    tl->addNode(new Token("'", SINGLE_QUOTE, lineNumber));
                    temp.clear();
                }
                else if (c == '"') {
                    state = double_quote;
                    tl->addNode(new Token("\"", DOUBLE_QUOTE, lineNumber));
                    temp.clear();
                }
                else if (isSingleToken(c, tl, state)) {
                    temp = c;
                    tl->addNode(new Token(temp, (Token_Type)mp[c], lineNumber));
                    temp.clear();
                }
                else if (isPotentialToken(c, tl, state)) {
                    temp.clear();
                    temp += c;
                    break;
                }
                else if (isalpha(c)) {
                    temp.clear();
                    temp += c;
                    state = identifier;
                }
                else if (isdigit(c)) {
                    temp.clear();
                    temp+=c;
                    state = digit;
                }
            break;
            case single_quote:
                if (c == '\'') {
                    state = start;
                    tl->addNode(new Token("'", SINGLE_QUOTE, lineNumber));
                }
                else {
                    state = single_quote_str;
                    temp += c;
                }
            break;
            case single_quote_str: 
                quoteStartLineNumber = lineNumber;
                if (c == '\'') {
                    state = start;
                    tl->addNode(new Token(temp, STRING, lineNumber));
                    tl->addNode(new Token("'", SINGLE_QUOTE, lineNumber));
                }
                else if(c == '\\'){
                    temp += c;
                    state = single_quote_escape_character;
                }
                else {
                    temp += c;
                }
            break;
            case single_quote_escape_character:
                if (c == 'x' || c == '"' || c == '\'' || c == 'a' || c == 'b' || c == 'n'){//There is a lot more Escape Characters.
                    string escape_character = "\\";
                    escape_character += c;
                    // tl->addNode(new Token(escape_character, ESCAPE_CHARACTER, lineNumber));
                    state = single_quote_str;
                }
                temp += c;
                break;
            case double_quote:
                quoteStartLineNumber = lineNumber;
                if (c == '"') {
                    state = start;
                    tl->addNode(new Token("\"", DOUBLE_QUOTE, lineNumber));
                }
                else {
                    state = double_quote_str;
                    temp += c;
                }
            break;
            case double_quote_str: 
                if (c == '"') {
                    state = start;
                    tl->addNode(new Token(temp, STRING, lineNumber));
                    tl->addNode(new Token("\"", DOUBLE_QUOTE, lineNumber));
                } // TODO: Insert escaped character handling for assignment 3, test5
                else if(c == '\\'){
                    temp += c;
                    state = double_quote_escape_character;
                }
                else {
                    temp += c;
                }
            break;
            case double_quote_escape_character:
                if (c == 'x' || c == '\'' || c == 'a' || c == 'b' || c == 'n'){//There is a lot more Escape Characters.
                    string escape_character = "\\";
                    escape_character += c;
                     // tl->addNode(new Token(escape_character, ESCAPE_CHARACTER, lineNumber));
                    state = double_quote_str;
                }
                temp += c;
                break;
            case lt: 
                if (c == '='){
                    tl->addNode(new Token("<=", LT_EQUAL, lineNumber));
                    state = start;
                }
                else {
                    tl->addNode(new Token("<", LT, lineNumber));
                    if (isSingleToken(c, tl, state)) {
                        temp.clear();
                        state = start;
                    }
                    else if (isPotentialToken(c, tl, state)) {
                        temp+=c;
                    }
                    else if (isalpha(c)) {
                        temp.clear();
                        temp += c;
                        state = identifier;
                    }
                    else if (isspace(c)) {
                        state = start;
                    }
                }
            break;
            case gt: 
                if (c == '=') {
                    tl->addNode(new Token(">=", GT_EQUAL, lineNumber));
                    state = start;
                }
                else {
                    tl->addNode(new Token(">", GT, lineNumber));
                    if (isSingleToken(c, tl, state)) {
                        temp.clear();
                        state = start;
                    }
                    else if (isPotentialToken(c, tl, state)) {
                        temp+=c;
                    }
                    else if (isalpha(c)) {
                        temp.clear();
                        temp += c;
                        state = identifier;
                    }
                    else if (isspace(c)) {
                        state = start;
                    }
                }
            break;
            case negative:
                if (isdigit(c)) {
                    temp+=c;
                    state = digit;
                }
                else if (isspace(c)) {
                    tl->addNode(new Token("-", MINUS, lineNumber));
                    state = start;
                    temp.clear();
                }
            break;
            case digit: {
                if (isdigit(c)) {
                    temp+=c;
                }
                else if (isSingleToken(c, tl, state)) {
                    tl->addNode(new Token(temp, INTEGER, lineNumber));
                    temp = c;
                    tl->addNode(new Token(temp, (Token_Type)mp[c], lineNumber));
                    temp.clear();
                    break;
                }
                else if (isPotentialToken(c, tl, state)) {
                    tl->addNode(new Token(temp, INTEGER, lineNumber));
                    temp.clear();
                }
                else if (isalpha(c)) {
                    std::cerr << "Syntax error on line " << lineNumber << ": invalid integer" << std::endl;
                    delete tl;
                    return nullptr;
                }
                else {
                    tl->addNode(new Token(temp, INTEGER, lineNumber));
                    state = start;
                    temp.clear();
                }
            }
            break;
            case bool_or: 
                if (c == '|') {
                    tl->addNode(new Token("||", BOOLEAN_OR, lineNumber));
                    state = start;
                }
                else if (isspace(c)){
                    std::cerr << "Syntax error at line " << lineNumber << ": invalid boolean or" << std::endl;
                }
            break;
            case bool_and:
                if (c == '&') {
                    tl->addNode(new Token("&&", BOOLEAN_AND, lineNumber));
                    state = start;
                }
                else if (isspace(c)){
                    std::cerr << "Syntax error at line " << lineNumber << ": invalid boolean and" << std::endl;
                }
            break;
            case bool_not:
                if (c == '='){
                    tl->addNode(new Token("!=", BOOLEAN_NOT_EQUAL, lineNumber));
                    state = start;
                    break;
                }
                else if(isalpha(c)) {
                    tl->addNode(new Token("!", BOOLEAN_NOT, lineNumber));
                    temp.clear();
                    temp = c;
                    state = identifier;
                }
                else if(isspace(c)){
                    std::cerr << "Syntax error at line " << lineNumber << ": invalid boolean not" << std::endl;
                }
            break;
            case identifier: 
                if(isSingleToken(c, tl, state)) {
                    tl->addNode(new Token(temp, IDENTIFIER, lineNumber));
                    temp = c;
                    tl->addNode(new Token(temp, (Token_Type)mp[c], lineNumber));
                    state = start;
                    temp.clear();
                    break;
                }
                else if (isPotentialToken(c, tl, state)) {
                    tl->addNode(new Token(temp, IDENTIFIER, lineNumber));
                    temp.clear();
                    state = start;
                    temp+=c;
                }
                else if (isspace(c)) {
                    tl->addNode(new Token(temp, IDENTIFIER, lineNumber));
                    state = start;
                    temp.clear();
                }
                else {
                    temp+=c;
                }
            break;
            case assign: 
                if(c == '=') {
                    tl->addNode(new Token("==", BOOLEAN_EQUAL, lineNumber));
                    state = start;
                    break;
                }
                else if(isspace(c)) {
                    tl->addNode(new Token("=", ASSIGNMENT_OPERATOR, lineNumber));
                    state = start;
                }
                else if(isalpha(c)) {
                    tl->addNode(new Token("=", ASSIGNMENT_OPERATOR, lineNumber));
                    state = identifier;
                }
                if(isSingleToken(c, tl, state)) {
                    tl->addNode(new Token(temp, IDENTIFIER, lineNumber));
                    temp = c;
                    tl->addNode(new Token(temp, (Token_Type)mp[c], lineNumber));
                    state = start;
                    temp.clear();
                    break;
                }
                else if (isPotentialToken(c, tl, state)) {
                    tl->addNode(new Token(temp, IDENTIFIER, lineNumber));
                    temp.clear();
                    state = start;
                    temp+=c;
                }
            default: 
            break;
        }
        i++;
    }
    if (state == single_quote_str || state == double_quote_str || state == single_quote_escape_character || state == double_quote_escape_character ) {
        eh.exitUnterminatedStr(quoteStartLineNumber);
    }
    if (state == identifier){
        // eh.exitStatementIncomplete(lineNumber, "Expected an SEMICOLON");
    }
    return tl;
}

// Additional Notes:
// - state and temp are currently local variables but could be refactored into the class header
