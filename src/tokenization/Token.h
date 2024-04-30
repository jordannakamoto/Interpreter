// Token.h
// Created by Nick Cabrales on 2/22/24.
// Edited by Jordan Nakamoto
//

#ifndef ASSIGNMENT2TOKENIZER_TOKEN_HPP
#define ASSIGNMENT2TOKENIZER_TOKEN_HPP

#include <string>

// Token Type List!!
namespace tdfa {
    enum Token_Type{
        // --------- TOKEN TYPES --------
        // Version 1
        // 
        // ------ default ---------------
        NONE, // 0
        // ------ identifier ------------
        IDENTIFIER,
        // ------ id letter types -------
        LETTER,                         // base unit : any A-Z character within an identifier
        LETTER_UNDERSCORE,              // Letter or _
        LETTER_DIGIT,                   // Letter or 0-9
        // --------- strings ------------
        STRING,
        CHARACTER,                      // base unit : any character within a string
        SINGLE_QUOTE,
        DOUBLE_QUOTE,
        // --------- numbers ------------
        DIGIT,                          // base unit : 0-9
        // math operators:
        PLUS, // 10
        MINUS,
        DIVIDE,
        ASTERISK,
        MODULO,
        CARET,
        // boolean operators
        LT,
        GT,
        LT_EQUAL,
        GT_EQUAL,
        BOOLEAN_AND, // 20
        BOOLEAN_OR,
        BOOLEAN_NOT,
        BOOLEAN_EQUAL,
        BOOLEAN_NOT_EQUAL,
        // compound:
        WHOLE_NUMBER,
        INTEGER,
        // --------- operators -----------
        ASSIGNMENT_OPERATOR,
        // --------- brackets ------------
        LEFT_PARENTHESIS,
        RIGHT_PARENTHESIS,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_BRACKET,
        RIGHT_BRACKET,
        // --------- delimiters -----------
        SEMICOLON,                        // endline delimiter
        COMMA,
        ESCAPE_CHARACTER,
        // --------- new ------------------
        BOOLEAN_TRUE,
        BOOLEAN_FALSE,
        // AST TYPES
        AST_FUNCTION_DECLARATION,
        AST_PROCEDURE_DECLARATION,
        AST_VARIABLE_DECLARATION,
        AST_ASSIGNMENT,
        AST_CALL,
        AST_IF,
        AST_ELSE,
        AST_FOR,
        AST_WHILE,
        AST_BEGIN_BLOCK,
        AST_END_BLOCK,
        AST_RETURN,
        AST_PRINTF
    };
}

using namespace tdfa;
// Class Token
class Token {
public:
    // Constructor
    Token();
    Token(std::string, Token_Type, int);
    Token(Token_Type); // Initialize with Token_Type
    Token* copy();

    // Attributes
    std::string tokenValue = "";
    Token_Type tokenType = Token_Type::NONE;
    int lineNumber;

    // Getters
    std::string getTokenValue();
    Token_Type getTokenType();          // returns type index like '0'
    std::string getTokenTypeString();   // returns name of type like 'NONE'
    int getTokenLineNumber();

    // Setters
    void set_TokenValue(std::string s);
    void set_TokenType(Token_Type type);
    void set_TokenLineNumber(int num);

    // Display Enum Entry as String for Debugging
    static std::string tokenTypeToString(Token_Type type);

    // ---- Notes ----
    // may want to attribute scope to the token itself subsequently
    // optionally we could use smaller datatypes for the value
    // ---------------

    //BRAYAN CREATED EVERYTHING BELOW THIS!
    bool &endOfFile();
    ~Token(){};

private:
    bool _eof; // TODO: I'm not sure we're using eof anymore
};

#endif //ASSIGNMENT2TOKENIZER_TOKEN_HPP

