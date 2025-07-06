#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;
ostringstream output_buffer;

/* FUNCTIONS */
void program();
int statement();
int expr(int& result);
int term(int& result);
int factor(int& result);
int number(int& result);
int var();

/* UTIL FUNCTIONS */
void prompt();
int init(); // 전역 변수 초기화
int lexeme();

/* TOKEN CODES */
#define ASSIGN 61
#define ADD 43
#define SUBTRACT 45
#define PRODUCT 42
#define LEFT_PAREN 40
#define RIGHT_PAREN 41

/* GLOBAL VARIABLES*/
string input_line;
istringstream iss;
string next_token;
unordered_map<string, int> symbol_table;

int main() {
    program();
    exit(0);
}
void program() { /* <program> -> { <statement> } */
    while (true) {
        prompt();

        int status = init(); // 사용자 입력 처리
        if (status < 0){
            exit(1);
        } else if (status == 0)
            break;
        
        int statement_loop = 1;
        while (statement_loop){
            if ((statement_loop = statement()) < 0) {
                output_buffer.str("");
                output_buffer.clear();
                cerr << "Syntax Error" << endl;
                break;
            }
        }
        if (!output_buffer.str().empty())
            cout << output_buffer.str() << endl;

        output_buffer.str("");
        output_buffer.clear();
        symbol_table.clear();
    }
}
int statement() { /* <statement> -> <var> = <expr> ; | print <var> ; */
    // RETURN VALUE
    //   정상 종료 시 0, 이어지는 statement가 존재할 경우 1, 실패 시 -1 (syntax error) 
    bool is_print = false;
    
    /* OPERATION */
    int result=0;
    string var_name;

    /* PRINT */ 
    if (next_token == "print"){
        is_print = true;
        /* print <var> ; */
        if (lexeme() <0){
            return -1;
        }
        if (var() < 0){
            return -1;
        } else 
            var_name = next_token;
        if (lexeme() < 0){
            return -1;
        }
    }
    else if (var() < 0){ // syntax error
        return -1;
    }
    /* ASSIGN */
    else {
        var_name = next_token;
        if (lexeme() < 0){
            return -1;
        }
        if (next_token == "=") {
            if (lexeme() < 0) {
                return -1;
            }
            if (expr(result) < 0) {
                return -1;
            }
        } else return -1;
    }

    if (next_token != ";") {
        return -1;
    }

    
    /* PRINT */
    if (is_print) {
        output_buffer << symbol_table[var_name] << " ";
    }
    /* ASSIGN */ 
    else {
        symbol_table[var_name] = result;
    }

    if (lexeme() < 0){
        return 0;
    } else return 1;
}
int expr(int& result) { /* <expr> -> <term> { + <term> | * <term> } */
    // RETURN VALUE
    //   성공 시 0, 실패 시 -1 
    
    if (term(result) < 0) {
        return -1;
    } 
    if (next_token == ";"){
        return 0;
    }
    
    /* OPERATION */
    while (next_token == "+" || next_token == "*") {
        string op = next_token;
        if (lexeme() < 0) {
            return -1;
        }

        int op_rhs;
        if (term(op_rhs) < 0) {
            return -1;
        }
        if (op == "+")  result += op_rhs;
        else if (op == "*") result *= op_rhs;
    }
    return 0;
}
int term(int& result) { /* <term> -> <factor> { - <factor> } */
    // RETURN VALUE
    //   성공 시 0, 실패 시 -1 
    if (factor(result) < 0) {
        return -1;
    }
    if (next_token == ";"){
        return 0;
    }
    while (next_token == "-") {
        if (lexeme() < 0) {
            return -1;
        }
        int op_rhs;
        if (factor(op_rhs) < 0) {
            return -1;
        } else {
            result -= op_rhs;
        }
    }
    return 0;
}
int factor(int& result) { /* <factor> -> [-] ( <number> | <var> | '(' <expr> ')' ) */
    // RETURN VALUE
    //   성공 시 0, 실패 시 -1 
    bool negative = false;

    if (next_token == "-") {
        negative = true;
        if (lexeme() < 0){
            return -1;
        }
    }
    if (next_token == "(") {
        if (lexeme() < 0) {
            return -1;
        }
        if (expr(result) < 0) {
            return -1;
        }
        if (next_token == ")") {
            if (lexeme() < 0) {
                return -1;
            }
        } else return -1;
    } else if (number(result) == 0) {
        if (lexeme() < 0){
            return -1;
        }
    } else if (var() == 0){
        string var_name = next_token;
        if (lexeme() < 0){
            return -1;
        }
        result = symbol_table[var_name];
    } else return -1;

    if (negative)
        result = -result;

    return 0;
}
int number(int& result) { /* <number> -> <digit> { <digit> } */
    // RETURN VALUE
    //  성공 시 0, 에러 시 -1
    for (char c: next_token){
        /* <digit> -> 0 | 1 | ... | 9 */
        if (!isdigit(c)){
            return -1;
        }
    }
    result = stoi(next_token);
    return 0;
}
int var(){ /* <var> -> <alphabet> { <alphabet> } */
    // RETURN VALUE
    //  성공 시 0, 에러 시 -1
    for (char c: next_token){
        /* <alphabet> -> a | b | ... | z */
        if (!islower(c)){
            return -1;
        }
    }
    return 0;
}
/* UTIL FUNCTIONS */
void prompt(){
    cout << ">> " ;
}
int init(){
    // RETURN VALUE
    //   성공 시 1, 개행 문자만 입력 시 0, 실패 시 -1 
    if (!getline(cin, input_line)){
        return -1;
    }
    if (input_line.empty()){
        return 0;
    }
    iss = istringstream(input_line);
    if (lexeme() < 0){
        return -1;
    }
    return 1;
}
int lexeme(){
    // RETURN VALUE
    //   성공 시 0, 실패 시 -1 
    if (!(iss >> next_token)){
        // end of line
        return -1;
    }
    return 0;
}