// CommentRemover.cpp
// Comment Removal PreProcessor and its DFA
#include "CommentRemover.h"

/* Design:
 *  - Initialize the CommentIdentifierDFA, which sets the state to START.
 *  - For each character in the input stream, call CommentRemover.processChar(char ch) to process the character through the DFA.
 *  - At any point, CommentIdentifierDFA.isActive() can be queried to determine if the DFA is within a comment or a potential initial symbol '/' */


/* Comment Identifier DFA -------------------------------------------- */
// Detect comments within a stream of characters
// - block comments: delimited by /* and */
// - line comments:  delimited by // and \n
/* ------------------------------------------------------------------ */
CommentIdentifierDFA::CommentIdentifierDFA() : state_(Comment_DFA_State::START) {} // Init at START State

// processChar
// Main State Machine transition method - Comment_DFA_State enum defined in header file.
void CommentIdentifierDFA::processChar(char ch) {
    switch (state_) {
        case Comment_DFA_State::START:
            if (ch == '"') {
                state_ = Comment_DFA_State::DOUBLE_QUOTE;
            }
            else if (ch == '/') {
                state_ = Comment_DFA_State::SLASH;
            }
            else if (ch == '*') {
                state_ = Comment_DFA_State::UNENDED_BLOCK_COMMENT_START;
            }
            if(isComment == true){
                isComment = false;
            }
            break;
        case Comment_DFA_State::SLASH:
            if (ch == '*') {
                isComment = true;
                state_ = Comment_DFA_State::BLOCK_COMMENT;
            } else if (ch == '/') {
                isComment = true;
                state_ = Comment_DFA_State::LINE_COMMENT;
            } else {
                isComment = false;
                state_ = Comment_DFA_State::START;
            }
            break;
        case Comment_DFA_State::BLOCK_COMMENT:
            if (ch == '*') {
                state_ = Comment_DFA_State::BLOCK_COMMENT_END;
            }
            break;
        case Comment_DFA_State::BLOCK_COMMENT_END:
            if (ch == '/') {
                state_ = Comment_DFA_State::START;
            } else if (ch != '*') {
                state_ = Comment_DFA_State::BLOCK_COMMENT;
            }
            break;
        case Comment_DFA_State::LINE_COMMENT:
            if (ch == '\n') {
                state_ = Comment_DFA_State::START;
            }
            break;
        case Comment_DFA_State::DOUBLE_QUOTE:
            if (ch == '"') {
                state_ = Comment_DFA_State::START;
            }
            break;
        case Comment_DFA_State::UNENDED_BLOCK_COMMENT_START:
            if (ch == '/') {
                state_ = Comment_DFA_State::UNENDED_BLOCK_COMMENT;
            }
            else if (ch == '*') { // stay in current state if another *
                break;
            }
            else {
                state_ = Comment_DFA_State::START;
            }
            break;
        default:
            break;
    }
}

// isActive - Flag that indicates that we've proceeded past the START state
bool CommentIdentifierDFA::isActive() const {
    return state_ != Comment_DFA_State::START;
}

// Debug Helper:
// Overloads the ostream operator << to output the current state of Comment_DFA_State as a readable string.
std::ostream &operator << (std::ostream& strm, Comment_DFA_State s){
    const std::string name[] = {"START", "SLASH", "BLOCK_COMMENT", "LINE_COMMENT", 
        "BLOCK_COMMENT_END", "DOUBLE_QUOTE", "UNENDED_BLOCK_COMMENT_START", "UNENDED_BLOCK_COMMENT"};
    return strm << name[s];
}

// class CommentRemover

/* Comment Remover -------------------------------------------------- */
// Usage: To be called by main.cpp as the pre-processor to remove comments before tokenization
//
// pre: FileHandler has opened an ifstream fileStream
// post: CommentRemover.result receives a std::string file representation with comments replaced with whitespace
/* ------------------------------------------------------------------ */

// Pass fileStream to constructor.
CommentRemover::CommentRemover(std::ifstream& file_stream) : fileStream(file_stream) {
    lineNumber = 1; // Init line count for error pinpointing
}

/* Core Method for Comment Removal */
// CommentRemover::processFile
// This method utilizes the CommentIdentifierDFA to detect comments within the source code.
// It operates by reading the file character-by-character, utilizing a buffer to temporarily store characters.
// - When the DFA identifies a sequence of characters as a comment, those characters are replaced with whitespace.
//   This modified content is then appended to the 'result' string, which holds the processed file content without comments.
// - Characters not part of a comment are directly appended to the 'result'.
//
// The method also implements error handling to identify unterminated comments or string literals, 
// flagging them with appropriate error messages.

void CommentRemover::processFile() {
    if (!fileStream.is_open()) {
        std::cerr << "Error: File stream is not open.\n";
        return;
    }

    CommentIdentifierDFA comment_dfa;

    char ch;
    // int startLine;
    bool inComment = false;
    while (fileStream.get(ch)) {
        comment_dfa.processChar(ch);
        if(ch == '\n') {
            lineNumber++;
        }
        // If we are potentially inside a comment, push the character into the buffer
        if(comment_dfa.isActive()){
            if (!inComment) {
                inComment = true;
            }
            buffer.push_back(ch);
        }
        // If the comment DFA is back at the START state, we have reached the end of a comment and need to flush the buffer
        else if (buffer.size() > 0){
            inComment = false;
            buffer.push_back(ch);
            if(comment_dfa.isComment == true){
                bufferToWhiteSpace();
            }
            transferBufferToFileContent();
        }
        else{
            result += ch;
        }

        // ERROR HANDLING while get(ch):
        // if the DFA has detected an unended block comment, we can exit the fileStream early with an error
        if (comment_dfa.getState() == Comment_DFA_State::UNENDED_BLOCK_COMMENT) {
            ErrorHandler::get().addError(lineNumber , "ERROR: program contains C-style, unterminated comment on line" + std::to_string(lineNumber), 0);
        }
    }

    // ERROR HANDLING after complete file read:
    // if we reach the last byte, and we're still in a double quote throw error

    Comment_DFA_State endState = comment_dfa.getState();
    // TODO: catch single quote?
    if (endState == Comment_DFA_State::DOUBLE_QUOTE) {
        ErrorHandler::get().addError(lineNumber , "ERROR: unterminated string literal on line " + std::to_string(lineNumber), 0);
    } // ... or C-style comment
    if (comment_dfa.isComment == true && buffer.size() > 0){
        ErrorHandler::get().addError(lineNumber , "ERROR: program contains C-style, unterminated comment on line" + std::to_string(lineNumber), 0);
    }
}

// BUFFER METHODS
// bufferToWhiteSpace - Buffer is converted to whitespace
void CommentRemover::bufferToWhiteSpace(){
    for(unsigned int i = 0; i < buffer.size(); i++){
        if(buffer[i] == '\n'){
            //lineNumber++;
            continue;
        }
        buffer[i] = ' ';
    }
}

// flushBuffer - Buffer is written to fileContent then cleared
void CommentRemover::transferBufferToFileContent(){
    for(unsigned int i = 0; i < buffer.size(); i++){
        result += buffer[i];
    }
    buffer.clear();
}

// debug method: printBuffer - prints file buffer which is used during the comment removal process
void CommentRemover::printBuffer(){
    std::cout << "BUFFER: ";
    for(unsigned int i = 0; i < buffer.size(); i++) {
        std::cout << buffer[i];
    }
    std::cout << std::endl;
}

// PRINT
// Prints result after the comment removal process
void CommentRemover::printCommentRemovedFile(){
    std::cout << result;
}

// CLIPBOARD
// Error Handling
// --- Direct Exit ---
// std::cout << "ERROR: program contains C-style, unterminated comment on line " << startLine << std::endl;
// exit(12);