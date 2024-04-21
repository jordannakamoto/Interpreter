// CommentRemover.h
// Contains 2 classes:
// Comment Remover and its state machine, CommentIdentifierDFA
// ---------------------------------------------------------------------------- //
// Comment Remover is designed for preprocessing C/C++ files
// for interpretation by removing line and block comments
// ---------------------------------------------------------------------------- //

#ifndef COMMENT_REMOVER_H
#define COMMENT_REMOVER_H
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stack>
#include "ErrorHandler.h"

/* State Enum */
namespace CDFA {
    enum Comment_DFA_State {
        START = 0,
        SLASH = 1,
        BLOCK_COMMENT = 2,
        LINE_COMMENT = 3,
        BLOCK_COMMENT_END = 4,
        DOUBLE_QUOTE = 5,
        UNENDED_BLOCK_COMMENT_START = 6,
        UNENDED_BLOCK_COMMENT = 7,
    };
}

using namespace CDFA;
std::ostream &operator << (std::ostream& strm, Comment_DFA_State s);

class CommentIdentifierDFA {
public:

    CommentIdentifierDFA();

    void processChar(char ch);

    bool isActive() const;
    bool isComment;

    Comment_DFA_State getState() const { return state_; }

private:
    Comment_DFA_State state_;
};

class CommentRemover {
public:
    CommentRemover(std::ifstream&);
    void processFile();
    void printBuffer();
    int lineNumber;
    std::string result;
    void printCommentRemovedFile();
private:
    std::ifstream& fileStream;
    std::vector<char> buffer;
    void bufferToWhiteSpace();
    void transferBufferToFileContent();
};


#endif // COMMENT_REMOVER_H

