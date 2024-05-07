// FileHandler
/* Manages file input/output. Produces a file output for each stage of the Interpreter. */

#include <fstream>
#include <iostream>
#include <vector>
#include "FileHandler.h"

// Constructor
FileHandler::FileHandler() {
    lineNumber = 1;
}

// ASSIGNMENT 1: Comment Removal --------------------------------------------------------------------- //

// openFile
void FileHandler::openFile(const std::string& filename) {
        fileStream.open(filename, std::ios::binary);
        if (!fileStream.is_open()) {
            std::cerr << "Error opening file: " << filename << "\n";
        }
        fileName = filename;
    }

// printInitialFile - reads and prints Original file stream then resets its position back to where it was before
// should be called before the start of main program procedures
void FileHandler::printInitialFile() {
        if (!fileStream) {
            std::cerr << "Stream is not open." << std::endl;
            return;
        }
        std::streampos originalPosition = fileStream.tellg(); // Save the original position of the file stream
        fileStream.seekg(0, std::ios::beg);                   // Reset to the start of the file
        std::string line;                                    // Read and print the file contents
        while (getline(fileStream, line)) {
            lineNumber++;
            std::cout << line << std::endl;
        }
        fileStream.clear();                                   // Restore the file stream's position and state
        fileStream.seekg(originalPosition);
        std::streampos newPosition = fileStream.tellg();      // Confirm the restoration
        if (newPosition != originalPosition) {
            std::cerr << "Failed to restore the stream position." << std::endl;
        }
}

// outputWithoutCommentsToFile - to be called by main after Procedure 1
// Writes the fileContent string to a file with the same name as the input file, but with "-comments_removed_output_file" appended to the end.
void FileHandler::outputWithoutCommentsToFile(CommentRemover cr) {
    std::string outputName = generateOutputFileName("comments_removed_output_file");
    std::ofstream outputFile(outputName); // Open File

    if (outputFile.is_open()) {
        outputFile << cr.result; // Write the string to the file
        outputFile.close();
        std::cout << "String has been written to " << outputName << " successfully." << std::endl;
    } else {
        std::cerr << "Unable to open " << outputName << " for writing." << std::endl;
    }
}
// ASSIGNMENT 2: Tokenizer ------------------------------------------------------------------------------- //

// outputTokenListToFile - to be called by main after Procedure 2
// Writes the linked list to a file with the same name as the input file, but with "-output_file.txt" appended to the end.
void FileHandler::outputTokenListToFile(TokenList* list) {
    std::string outputName = generateOutputFileName("tokenlist_output_file");
    std::ofstream outputFile(outputName); // Open File
    TokenList::Node* temp;
    temp = list->head;
    if (outputFile.is_open()) {
        while(temp != nullptr) {
            outputFile << "Token type: " << temp->getToken()->getTokenTypeString() << "\n";
            outputFile << "Token:      " << temp->getToken()->getTokenValue() << "\n";
            temp=temp->getNext();
            outputFile << "\n";
        }
        outputFile << "\n";
        outputFile.close();
        std::cout << "String has been written to " << outputName << " successfully." << std::endl;
    } else {
        std::cerr << "Unable to open " << outputName << " for writing." << std::endl;
    }
}
// ASSIGNMENT 3: CST Recursive Descent ----------------------------------------------------------------------- //

void FileHandler::outputCSTToFile(ConcreteSyntaxTree* tree) {
    std::string outputName = generateOutputFileName("cst_output_file");
    std::ofstream outputFile(outputName); // Open File

    if (!outputFile.is_open()) {
        std::cerr << "Unable to open " << outputName << " for writing." << std::endl;
        return;
    }
    ConcreteSyntaxTree::Node* current = tree->head;
    while ( current != nullptr ) {

        // Print current node's token value
        outputFile << current->getToken()->getTokenValue();

        if (current->getNextSibling() != nullptr) {
            outputFile << " "; // Add a space if there is a next sibling
            current = current->getNextSibling();
        } else {
            if(current->getNextChild() != nullptr) { // Make sure not to print last newline...
                outputFile << "\n"; // Indicate moving down a level with a newline   
            }
            current = current->getNextChild();
        }
    };
    outputFile.close();
    std::cout << "String has been written to " << outputName << " successfully." << std::endl;
}

// ASSIGNMENT 4: Symbol Table -------------------------------------------------------------------------------- //
void FileHandler::outputSymbolTableToFile(SymbolTable* st) {
    std::string outputName = generateOutputFileName("symboltable_output_file");
    std::ofstream outputFile(outputName); // Open File

    if(outputFile.is_open()) {
        auto oldBuf = std::cout.rdbuf();
        std::cout.rdbuf(outputFile.rdbuf());
        st->printTable();
        std::cout.rdbuf(oldBuf);
        outputFile.close();
    }
}

// ASSIGNMENT 5: Abstract Syntax Tree ------------------------------------------------------------------------ //
void FileHandler::outputASTToFile(AbstractSyntaxTree ast){
    std::string outputName = generateOutputFileName("ast_output_file");
    std::ofstream outputFile(outputName); // open file

    if (!outputFile.is_open()) {
        std::cerr << "Unable to open " << outputName << " for writing." << std::endl;
        return;
    }

    AbstractSyntaxTree::Node* current = ast.head;

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
            outputFile << " ";
            if(!isFunctionCall){ // insert the start of list delimiter if not a function call
                outputFile << "(";
            }
        }
        if(sib_counter == 2  && isFunctionCall){
            outputFile << " ("; // otherwise insert the start of list delimiter at sib 2
        }
        outputFile << current->getToken()->getTokenValue();
        if (current->getNextSibling() != nullptr) {
            sib_counter++;
            // Insert list separators until last item in line
            // if isFunctionCall, start printing separators at sib3 instead of sib2
            if(sib_counter == 2 && !isFunctionCall && current->getNextChild() == nullptr){
                outputFile << "|";
            }
            if(sib_counter > 2 && current->getNextChild() == nullptr){
                outputFile << "|";
            }
            current = current->getNextSibling();
        } else {
            // If we were on a line with a list, close the list
            if (sib_counter >= 1 ){
                    outputFile << ")";
            }
            // If we reached EOF, don't print a newline
            if(current->getNextChild() != nullptr){ 
                outputFile << "\n"; // Otherwise for nextChild, indicate moving down a level with a newline
            }
            sib_counter = 0;
            current = current->getNextChild();
        }
    }

    outputFile.close();
    std::cout << "Abstract Syntax Tree has been written to " << outputName << " successfully." << std::endl;
}
// ASSIGNMENT 6: Interpreter --------------------------------------------------------------------------------- //
void FileHandler::outputInterpreterToFile(Interpreter* interpreter) {
    std::string outputName = generateOutputFileName("interpreter_output_file");
    std::ofstream outputFile(outputName); // Open File

    if(outputFile.is_open()) {
        outputFile << interpreter->outstring;
    }
    outputFile.close();
    std::cout << "Interpreter output has been written to " << outputName << " successfully." << std::endl;
}


// ----------------------------------------------------------------------------------------------------------- //

// generateOutputFileName()
// returns a string with the proper output file path + name
// ex. "inputs/test1.c" -> "program-outputs/test1-output_file.txt"
std::string FileHandler::generateOutputFileName(std::string descriptor){
    size_t lastIndex = fileName.find_last_of(".");
    std::string outputName = fileName.substr(0, lastIndex);

    // Change path to new output directory
    std::string toSearch = "inputs";
    std::string replaceWith = "program-outputs";
    size_t pos = outputName.find(toSearch);
    if (pos != std::string::npos) { // if "inputs" is found
        outputName.replace(pos, toSearch.length(), replaceWith);
    }
    // std::cout << outputName << std::endl;

    outputName += "-" + descriptor + ".txt";
    return outputName;
}

// Getters
std::string FileHandler::getFileName(){
    return fileName;
};
