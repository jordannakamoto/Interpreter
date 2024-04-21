// Written by Jacob Jaffe and Brayan Vizcaino
// Edited by Jordan Nakamoto Mar-23
// Edited by Brayan Vizcaino Mar-25

// Tokenizer
// - utilizes DFA state machine to recognize token types and their proper handling
// - TODO: I don't think we need to pass in the tokenlist for the isPotentialToken() method and isSingleToken() method

#ifndef TOKENIZER_TOKENIZER_H
#define TOKENIZER_TOKENIZER_H

#include <string>
#include <fstream>
#include <iostream>
#include <cctype>
#include <unordered_map>

#include "Token.h"
#include "TokenList.h"
#include "ErrorHandler.h"

using namespace std;

enum Tokenizer_DFA_State {
    start = 0,
    single_quote = 1,
    double_quote = 2,
    negative = 3,
    digit = 4,
    single_quote_str = 5,
    double_quote_str = 6,
    lt = 7,
    gt = 8,
    assign = 11,
    bool_and = 12,
    bool_or = 13,
    identifier = 14,
    bool_equal = 15,
    bool_not = 16,
    double_quote_escape_character = 17,
    single_quote_escape_character = 18,
    end = 1000,
    fail = 9000,
};

class Tokenizer {
public:
    Tokenizer(std::string);
    Token getToken();
    void SetString();
    TokenList* TokenizeToLinkedList();

private:
    ErrorHandler &eh = ErrorHandler::get();
    unsigned int i;//Iterator!
    int lineNumber;
    std::string fileStr;

    bool charOfInterest(char c);
    bool isSingleToken(char, TokenList*, Tokenizer_DFA_State&);
    bool isPotentialToken(char, TokenList*, Tokenizer_DFA_State&);

};

#endif //TOKENIZER_TOKENIZER_HPP
