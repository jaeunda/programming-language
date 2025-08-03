# PL Projects: Recursive-Descent Parsers
This repository contains two recursive-descent parser implementations written in `Python` and `C++`.


## Projects Overview
### [pl-p1](./pl-p1)
A basic expression-based interpreter with variable assignment and print support. Assumes all undeclared variables start at value 0.
- Arithmetic Operators: `+`, `-`, `*`
- Expression Associativity: Left-associative
- Output: `print` of variable values
- No type declarations or control flow


### [pl-p2](./pl-p2)
An extended interpreter including variable declarations, conditionals, and loops.
- **Mandatory** variable declaration using `integer`
- Control flow: `if-else`, `while`
- Relational operators: `==`, `!=`, `<`, `>`
- Arithmetic expressions and nested blocks supported


## Common Requirements
- All inputs are space-separated
- Input ends when a blank line is entered
- Syntax errors are reported as: `Syntax Error!`


## Directory Structure
```
.  
├── README.md 
├── pl-p1/ # Project 1: Expression-based parser  
│ └── rd-parser.cpp  
└── pl-p2/ # Project 2: Extended parser with control flow  
  ├── rd-parser.cpp 
  └── rd-parser.py 
```


## Usage
### pl-p1
```
g++ pl-p1/rd-parser.cpp -o rd-parser
./rd-parser
```

### pl-p2
#### C++
```
g++ pl-p2/rd-parser.cpp -o rd-parser
./rd-parser
```
#### Python
```
python3 pl-p2/rd-parser.py
```
