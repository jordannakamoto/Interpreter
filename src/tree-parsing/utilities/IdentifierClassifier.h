// Identifier Classifier
// For use by Recursive Descent Parser
//----------------------------------------------------------------
// - Takes an IDENTIFIER token and determines what reserved keyword group it belongs to
// - If it isn't a reserved keyword, it should be considered a Valid Variable Name

#ifndef IDENTIFIER_CLASSIFIER_H
#define IDENTIFIER_CLASSIFIER_H

#include <string>
#include <unordered_set>

class IdentifierClassifier {
public:
    static bool name(const std::string& identifier);
    static bool procedure(const std::string& identifier);
    static bool function(const std::string& identifier);
    static bool data(const std::string& identifier);
    static bool control(const std::string& identifier);
    static bool subroutine(const std::string& identifier);
};

#endif // IDENTIFIER_CLASSIFIER_H
