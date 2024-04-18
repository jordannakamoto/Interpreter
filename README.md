## Assignment 5: Abstract Syntax Tree
Nick Cabrales, Brayan Vizcaino, Jordan Nakamoto, Jacob Jaffe

### Shunting Yard Algorithm demonstrated in:
ShuntingYard.cpp
AbstractSyntaxTree.cpp > convertInfixToPostfix

### Construction of AST as binary singly linked list in:
AbstractSyntaxTree.cpp

### Build Instructions
* Makefile will build executable as "app" in root directory
* tests/inputs directory contains tests1-5 .c code
* tests/outputs directory contains expected output .pdf and converted .txt files with our chosen output format
* tests/program-outputs contains the outfiles produced by FileHandler.cpp

* Shell script runall will run ./app for all test .c files from tests/input directory
* Shell script testall will run diff for all tests/program-outputs files against corresponding .txts in tests/outputs
  
Example run:
`make all`
`./app tests/inputs/test1.c`

or chmod x the shell scripts
`./runall`
`./testall`

Output text formatting will display during execution, here it is again below.
```
--------------------------------------------------------
Print Format:
        Siblings printed on same line
        ()  surrounds Operators/Operands/Arguments
        |   separates Sibling Group Elements
        Children of last element printed on next line
--------------------------------------------------------
```