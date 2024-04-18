### Build Instructions
* Makefile will build executable as "app" in root directory

Rewrite
```
* tests/inputs directory contains tests1-5 .c code
* tests/outputs directory contains expected output .pdf and converted .txt files with our chosen output format
* tests/program-outputs contains the outfiles produced by FileHandler.cpp
```

* Shell script runall will run ./app for all test .c files from tests/input directory
* Shell script testall will run diff for all tests/program-outputs files against corresponding .txts in tests/outputs
  
Example run:
`make all`
`./app tests/inputs/test1.c`

or chmod x the shell scripts
`./runall`
`./testall`
