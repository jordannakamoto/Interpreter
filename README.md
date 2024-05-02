# Programming Language Interpreter
An Interpreter for a simple C-Like language written in C++

# Stages
* Comment Removal
* Tokenization (definition and identification of tokens)
* Parsing into Concrete Syntax Tree (syntax checking and statement modeling)
* Parsing into Abstract Syntax Tree (program instruction modeling)
* Symbol Table (function, variable, parameter list, scope modeling)
* Interpretation (runtime evaluation, stack memory management, and control flow)

### Build Instructions
* Makefile will build executable as "app" in root directory

Example run:
`make all`
`./app tests/inputs/test1.c`

* Shell script runall will run ./app for all test .c files from tests/input directory
* Shell script testall will run diff for all tests/program-outputs files against corresponding .txts in tests/outputs
  

or chmod x the shell scripts
`./runall`
`./testall`


## Design Philosophy
A general C++ programming pattern was used for both ease of development and program flow.
* Each **Stage** of parsing or processing has a class, for example `Tokenizer`
* Getter/Setter functions access class functionality
* The class is called by main.cpp during each stage along with its printing and output file methods
* A Stage class may be accompanied by utility classes or requisite data structures defined or linked in the header file
* Each stage .cpp has a primary procedure of a descriptive name which performs its task

  
<br><br> **The general class template is as follows:**
* Generally each stage works with a **Tree** of elements represented by a **Linked List** arrangement of **Nodes**
* Sibling-Child links are used
* Siblings form a group of elements which represent a unit of that Stage's responsibility
* For example the assignment: `n = 100` is constructed as 3 siblings
* Child relationships represent Siblings or statement units connected as descendants from the root
* A **Node** struct is used to define a Tree element to describe its relative position and its data storage
* **Tokens** as a storage mechanism are reused throughout the program to reduce the complexity of type management.
* **enums** are used to describe elementary types or states and are derived from rough DFA based analysis.
* A Token stores the data **Type**, the data **Value**, and its location from which we can produce handling rules.
* A main class body procedure constructs the Stage's Tree and is composed of various **handler methods**.
* switch case or if/else controls are used to branch the body procedure based on incoming Sibling-Child Node **relationships** or statements from the previous Stage
* A preprocessing phase may occur before the body
* The **print** and **File Handler** methods are defined below the body.