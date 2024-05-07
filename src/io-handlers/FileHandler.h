// FileHandler.h
#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "CommentRemover.h"
#include "TokenList.h"
#include "ConcreteSyntaxTree.h"
#include "SymbolTable.h"
#include "ErrorHandler.h"
#include "AbstractSyntaxTree.h"
#include "Interpreter.h"

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

class FileHandler {
public:
    FileHandler();

 	std::ifstream fileStream;

    void openFile(const std::string& filename);
    
    // Print initial file from stream before any main program actions
    void printInitialFile();

    void outputWithoutCommentsToFile(CommentRemover);
    void outputTokenListToFile(TokenList*);
    void outputCSTToFile(ConcreteSyntaxTree*);
    void outputSymbolTableToFile(SymbolTable* st);
    void outputASTToFile(AbstractSyntaxTree ast);
    void outputInterpreterToFile(Interpreter* interpreter);

    // Helper
    std::string generateOutputFileName(std::string);

    // Getters
    std::string getFileName();

    ~FileHandler() {
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }

private:
    std::string fileName;
    int lineNumber;
};

#endif // FILEHANDLER_H

