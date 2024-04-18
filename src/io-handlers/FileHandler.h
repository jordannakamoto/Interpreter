// FileHandler.h
#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "../tokenization/CommentRemover.h"
#include "../tokenization/TokenList.h"
#include "../tree-parsing/ConcreteSyntaxTree.h"
#include "../interpreter/SymbolTable.h"
#include "ErrorHandler.h"
#include "../tree-parsing/AbstractSyntaxTree.h"

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
